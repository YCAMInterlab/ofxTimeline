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
	enable();
    ofAddListener(ofEvents().update, this, &ofxTLVideoTrack::update);
    ofAddListener(ofEvents().exit, this, &ofxTLVideoTrack::exit);
    startThread();
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

    lock();
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

    videoThumbs = newThumbs;
    unlock();
}


void ofxTLVideoTrack::loadMovie(string moviePath){

    
    ofPtr<ofVideoPlayer> newPlayer = ofPtr<ofVideoPlayer>(new ofVideoPlayer());    
    if(newPlayer->loadMovie(moviePath)){
//        thumbDirectory = ofFilePath::removeExt(moviePath) + "_thumbs/";
//        ofDirectory checkCreateDirectory(thumbDirectory);
//        if (!checkCreateDirectory.exists()) {
//            ofLogError("ofxTLVideoTrack -- Directory " + thumbDirectory + " doesn't exist! Creating");
//            checkCreateDirectory.create(true);
//        }
        setPlayer( newPlayer );
    }
    else {
        ofLogError("ofxTLVideoTrack::loadMovie -- movie load failed: " + moviePath);
    }
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
        
        //if(!ofGetMousePressed() && (thumbnailUpdatedWidth != getDrawRect().width || thumbnailUpdatedHeight != getDrawRect().height) ) {
        	//generateVideoThumbnails();	
        //}
        
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

//void ofxTLVideoTrack::setVideoPlayer(ofVideoPlayer& newPlayer, string thumbDir){
//	setVideoPlayer(&newPlayer, thumbDir);
//}

//void ofxTLVideoTrack::setVideoPlayer(ofVideoPlayer* newPlayer, string thumbDir){
//    if(newPlayer == NULL ||!newPlayer->isLoaded()){
//        ofLogError("ofxTLVideoTrack::setVideoPlayer -- Null or unloaded player set sent");
//        return;
//    }
//    
//    player = newPlayer;	
//	ofDirectory checkCreateDirectory(thumbDir);
//	if (!checkCreateDirectory.exists()) {
//		ofLogError("ofxTLVideoTrack -- Directory " + thumbDir + " doesn't exist! Creating");
//		checkCreateDirectory.create(true);
//	}
//	
//	if(inFrame == -1){
//		cout << "reseting in out" << endl;
//		inFrame = 0;
//		outFrame = player->getTotalNumFrames();
//	}
//	
//	//TODO: check out
//	videoThumbs.clear();
//	thumbDirectory = thumbDir;
//	for(int i = 0; i < player->getTotalNumFrames(); i++){
//		ofxTLVideoThumb t;
//		t.setup(i, thumbDir);
//		videoThumbs.push_back(t);
//	}
//	
//	videoThumbs[0].visible = true;
//	generateThumbnailForFrame(0);
//	calculateFramePositions();
//	generateVideoThumbnails();
//}




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
//			cout << "setting percent complete " << 
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

void ofxTLVideoTrack::threadedFunction(){

	while(isThreadRunning()){
        if(!currentlyZooming && thumbsEnabled && backthreadedPlayer != NULL && backthreadedPlayer->isLoaded()){
            lock();
            
            for(int i = 0; i < videoThumbs.size(); i++){
                if(!videoThumbs[i].loaded){

                    backthreadedPlayer->setFrame(videoThumbs[i].framenum);			
                    backthreadedPlayer->update();                
                    videoThumbs[i].create(backthreadedPlayer->getPixelsRef());
                    
                    break;
                }
            }
            unlock();
        }
		
        ofSleepMillis(50);     
    }
}

//void ofxTLVideoTrack::generateThumbnailForFrame(int i){
//	if(!videoThumbs[i].loaded){
//		if(videoThumbs[i].exists){
//			videoThumbs[i].load();
//		}
//		else {
			//TODO: incorporate offset here
//            if(!player->isPlaying()){
//                player->play();
//                player->setSpeed(0);
//            }
//			player->setFrame(videoThumbs[i].framenum);			
//			player->update();
//
//			ofImage frameImage;
//			frameImage.setFromPixels(player->getPixelsRef());			
//			videoThumbs[i].create(frameImage);
//		}
//	}
//}

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

//void ofxTLVideoTrack::purgeOldThumbnails(){
//	for(int i = 0; i < videoThumbs.size(); i++){
//		//TODO
//	}
//}

void ofxTLVideoTrack::exit(ofEventArgs& args){
    stopThread();	
}

