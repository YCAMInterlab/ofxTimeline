/*
 *  ofxTLVideoTrack.cpp
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLVideoTrack.h"
#include "ofxTimeline.h"

ofxTLVideoTrack::ofxTLVideoTrack() 
	: ofxTLImageTrack()
{
	thumbsEnabled = true;
	currentLoop = 0;
	inFrame = -1;
	outFrame = -1;
    currentlyZooming = false;
}

ofxTLVideoTrack::~ofxTLVideoTrack(){

}

void ofxTLVideoTrack::setup(){
    ofxTLImageTrack::setup();
    ofAddListener(ofEvents().exit, this, &ofxTLVideoTrack::exit);
    startThread();
}

void ofxTLVideoTrack::enable(){
    ofxTLImageTrack::enable();
    ofAddListener(ofEvents().update, this, &ofxTLVideoTrack::update);
}

void ofxTLVideoTrack::disable(){
    ofxTLImageTrack::enable();
    ofRemoveListener(ofEvents().update, this, &ofxTLVideoTrack::update);
}

void ofxTLVideoTrack::update(ofEventArgs& args){
	if(player == NULL){
		return;
	}
	
   	if(player->isPlaying() && player->getSpeed() > 0.0){
		
        //cout << " is playing player frame " << player->getCurrentFrame() << " current frame " << getCurrentFrame() << endl;
		if(timeline->getIsFrameBased()){
            
			if(player->getCurrentFrame() < inFrame || player->getCurrentFrame() > outFrame){
                //cout << "reset in frame from " << player->getCurrentFrame() << endl;
				player->setFrame(inFrame);
                //cout << "	to: " << player->getCurrentFrame() << endl;
			}
			
			if(lastFrame > player->getCurrentFrame()){
				currentLoop++;
                //cout << "LOOPED! with last frame " << lastFrame << " " << player->getCurrentFrame() << " current loop " << currentLoop << endl;
			}
			
			if(timeline->getOutFrame() < getCurrentFrame() || timeline->getInFrame() > getCurrentFrame() ){				
				if(timeline->getInFrame() > player->getCurrentFrame() && timeline->getLoopType() == OF_LOOP_NONE){
					player->stop();
				}
				else {
					//player->setFrame( timeline->getInFrame() % player->getTotalNumFrames());
					selectFrame(timeline->getInFrame());
				}
			}
            
			timeline->setCurrentFrame(getCurrentFrame());
			lastFrame = player->getCurrentFrame();
		}
		else{
			if(timeline->getOutTime() < player->getPosition()*player->getDuration() || timeline->getInTime() > player->getPosition()*player->getDuration() ){
				player->setFrame(timeline->getInOutRange().min * player->getTotalNumFrames());
			}
			timeline->setCurrentTime( player->getPosition() * player->getDuration());
		}
	}
}


void ofxTLVideoTrack::framePositionsUpdated(vector<ofxTLVideoThumb>& newThumbs) {

    for(int i = 0; i < newThumbs.size(); i++){
		newThumbs[i].useTexture = false;        
    }

    //6) copy old textures thumbs over if they are still used, delete them if they are not
    for(int i = 0; i < videoThumbs.size(); i++){
        for(int j = 0; j < newThumbs.size(); j++){
            if(videoThumbs[i].framenum == newThumbs[j].framenum && videoThumbs[i].loaded){
                //newThumbs[j].create(videoThumbs[i].thumb->getPixelsRef());
                newThumbs[j].thumb = videoThumbs[i].thumb;
                newThumbs[j].loaded = true;
                break;
            }
        }
    }
    
	backLock.lock();
    backThumbs = newThumbs;
	backLock.unlock();
    
	lock();
    videoThumbs = newThumbs;
    unlock();
}

void ofxTLVideoTrack::threadedFunction(){
    
	while(isThreadRunning()){
        backLock.lock();
        if(!currentlyZooming && thumbsEnabled && backthreadedPlayer != NULL && backthreadedPlayer->isLoaded()){
            for(int i = 0; i < backThumbs.size(); i++){
                if(!backThumbs[i].loaded){
                    if(currentlyZooming){
                        break;
                    }
                    backthreadedPlayer->setFrame(backThumbs[i].framenum);			
                    backthreadedPlayer->update();                
                    if(currentlyZooming){
                        break;
                    }                    
                    backThumbs[i].create(backthreadedPlayer->getPixelsRef());
                    
                    lock();
                    videoThumbs[i] = backThumbs[i];
                    unlock();

                }
            }
        }
        backLock.unlock();
        
        ofSleepMillis(50);     
    }
}

bool ofxTLVideoTrack::load(string moviePath){

    ofPtr<ofVideoPlayer> newPlayer = ofPtr<ofVideoPlayer>(new ofVideoPlayer());    
    if(newPlayer->loadMovie(moviePath)){
        setPlayer( newPlayer );
        return true;
    }
    else {
        ofLogError("ofxTLVideoTrack::loadMovie -- movie load failed: " + moviePath);
    }
	return false;
}

void ofxTLVideoTrack::setPlayer(ofVideoPlayer& newPlayer){
	setPlayer(ofPtr<ofVideoPlayer>( &newPlayer ));    
}

void ofxTLVideoTrack::setPlayer(ofPtr<ofVideoPlayer> newPlayer){
    player = newPlayer;
    if(player->isLoaded()){
        calculateFramePositions();
        backthreadedPlayer = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
        backthreadedPlayer->setUseTexture(false);
        backthreadedPlayer->loadMovie(player->getMoviePath());
		inFrame = 0;
		outFrame = player->getTotalNumFrames();
    }
    else {
		ofLogError("ofxTLVideoTrack::setPlayer -- setting a video player before loading movie doesn't work!");
    }
}

ofPtr<ofVideoPlayer> ofxTLVideoTrack::getPlayer(){
    return player;
}

void ofxTLVideoTrack::draw(){
	
	if(player == NULL){
		return;
	}
	
	ofPushStyle();
	
	if(thumbsEnabled && getDrawRect().height > 10){
        
		ofSetColor(255);        
        lock();
		for(int i = 0; i < videoThumbs.size(); i++){

            if(videoThumbs[i].thumb != NULL){
                if(videoThumbs[i].loaded &&!videoThumbs[i].thumb->isUsingTexture()){
                    videoThumbs[i].thumb->setUseTexture(true);
                    videoThumbs[i].thumb->update();
                }

                videoThumbs[i].thumb->draw(videoThumbs[i].displayRect);
            }
            else{
                ofPushStyle();
                ofSetColor(0);
                ofRect(videoThumbs[i].displayRect);
                ofPopStyle();
            }
		}
		unlock();
        
		for(int i = 0; i < videoThumbs.size(); i++){

            if (!thumbsEnabled) {
                ofFill();
                ofSetColor(0);
                ofRect(videoThumbs[i].displayRect);
            }
            
            ofNoFill();
            ofSetColor(255, 150, 0);
            ofDrawBitmapString(ofToString(videoThumbs[i].framenum), videoThumbs[i].displayRect.x+5, videoThumbs[i].displayRect.y+15);
            ofRect(videoThumbs[i].displayRect);
		}		
	}
	
    //TODO: draw prettier
	int selectedFrameX = screenXForIndex(selectedFrame);
	ofSetColor(0, 125, 255);
	ofLine(selectedFrameX, bounds.y, selectedFrameX, bounds.y+bounds.height);
	ofDrawBitmapString("frame " + ofToString(selectedFrame), selectedFrameX, bounds.y+30);
	ofDrawBitmapString("seconds " + ofToString(player->getPosition()*player->getDuration()), selectedFrameX, bounds.y+48);
	
	if(inFrame != -1){
		ofSetLineWidth(2);
		ofSetColor(timeline->getColors().highlightColor);
		int inFrameX  = screenXForIndex(inFrame);
		int outFrameX = screenXForIndex(outFrame);
		ofLine(inFrameX, bounds.y, inFrameX, bounds.y+bounds.height);
		ofLine(outFrameX, bounds.y, outFrameX, bounds.y+bounds.height);
		ofSetColor(timeline->getColors().keyColor);
		ofDrawBitmapString("IN:  " + ofToString(inFrameX),  inFrameX  + 5, bounds.y + 10);
		ofDrawBitmapString("OUT: " + ofToString(outFrameX), outFrameX + 5, bounds.y + bounds.height - 20);
	}
	
	ofPopStyle();
}

void ofxTLVideoTrack::setInFrame(int in){
	inFrame = in;
}

void ofxTLVideoTrack::setOutFrame(int out){
	outFrame = out;
}

void ofxTLVideoTrack::mousePressed(ofMouseEventArgs& args){
	ofxTLTrack::mousePressed(args);
	if(getDrawRect().inside(args.x, args.y)){
		timeline->unselectAll();
	}
}

void ofxTLVideoTrack::mouseMoved(ofMouseEventArgs& args){
	ofxTLTrack::mouseMoved(args);
}

void ofxTLVideoTrack::mouseDragged(ofMouseEventArgs& args, bool snapped){
	if(bounds.inside(args.x, args.y)){
		selectFrame( indexForScreenX(args.x) );
		if(timeline->getMovePlayheadOnDrag()){
			timeline->setPercentComplete(screenXtoNormalizedX(args.x, zoomBounds));
		}
	}
}

bool ofxTLVideoTrack::isLoaded(){
	return player != NULL && player->isLoaded();
}

//width and height of image elements
float ofxTLVideoTrack::getContentWidth(){
    return isLoaded() ? player->getWidth() : 0;  
}

float ofxTLVideoTrack::getContentHeight(){
    return isLoaded() ? player->getHeight() : 0;    
}

void ofxTLVideoTrack::mouseReleased(ofMouseEventArgs& args){
    
}

void ofxTLVideoTrack::keyPressed(ofKeyEventArgs& args){
	if(hover){
		if(args.key == OF_KEY_LEFT){
			selectFrame(MAX(selectedFrame-1, 0));
		}
		else if(args.key == OF_KEY_RIGHT){
			selectFrame(MIN(selectedFrame+1, videoThumbs.size()-1));		
		}
	}	
}

int ofxTLVideoTrack::selectFrame(int frame){
//	selectedFrame = frame % videoThumbs.size();
	selectedFrame = inFrame + (frame % (outFrame - inFrame));
	lastFrame = selectedFrame;
	currentLoop = frame / (outFrame-inFrame);
//	cout << "selecting frame " << selectedFrame << endl;
	player->setFrame(selectedFrame);
	timeline->flagUserChangedValue();
	//cout << "selecting frame " << frame << " video frame " << selectedFrame << " current loop " << currentLoop << " duration " << player->getTotalNumFrames() << " timeline duration " << timeline->getDurationInFrames() << endl;
	return selectedFrame;
}



void ofxTLVideoTrack::toggleThumbs(){
	thumbsEnabled = !thumbsEnabled;
}

int ofxTLVideoTrack::getCurrentFrame(){
	return (player->getCurrentFrame() - inFrame) + currentLoop*(outFrame-inFrame);
}

float ofxTLVideoTrack::getCurrentTime(){
	return player->getPosition() * player->getDuration();
}

int ofxTLVideoTrack::getSelectedFrame(){
	return selectedFrame + currentLoop * (outFrame-inFrame);
}

void ofxTLVideoTrack::exit(ofEventArgs& args){
    stopThread();	
}

