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
	currentLoop = 0;
	inFrame = -1;
	outFrame = -1;
    currentlyPlaying = false;
}

ofxTLVideoTrack::~ofxTLVideoTrack(){

}

void ofxTLVideoTrack::setup(){
    ofxTLImageTrack::setup();
    ofAddListener(ofEvents().exit, this, &ofxTLVideoTrack::exit);
    startThread();
}

void ofxTLVideoTrack::enable(){
	if(!enabled){
		ofxTLImageTrack::enable();
		ofAddListener(ofEvents().update, this, &ofxTLVideoTrack::update);
		ofAddListener(events().playheadScrubbed, this, &ofxTLVideoTrack::playheadScrubbed);
	}
}

void ofxTLVideoTrack::disable(){
	if(enabled){
		stop();
		ofxTLImageTrack::disable();
		ofRemoveListener(ofEvents().update, this, &ofxTLVideoTrack::update);
		ofRemoveListener(events().playheadScrubbed, this, &ofxTLVideoTrack::playheadScrubbed);
	} 
}

bool ofxTLVideoTrack::togglePlay(){
	
    if(!isLoaded()) return false;
    
    if(isPlaying()){
        stop();
        return false;
    }
    else{
        play();
        return true;
    }
}

void ofxTLVideoTrack::play(){
    if(isLoaded()){
        cout << " current video frame is " << player->getCurrentFrame() << " current video time is " << ofxTimecode::timecodeForSeconds(player->getDuration()*player->getPosition()) << " current timeline time is " << timeline->getCurrentTime() << endl;
        if(!player->isPlaying()){
            player->play();
        }
        player->setSpeed(1.0);
        
        
        currentlyPlaying = true;
        ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
        ofNotifyEvent(events().playbackStarted, args);
    }
}

void ofxTLVideoTrack::stop(){
    if(isLoaded()){
        player->setSpeed(0.0);
        ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
        ofNotifyEvent(events().playbackEnded, args);
        currentlyPlaying = false;    
    }
}

bool ofxTLVideoTrack::isPlaying(){
	return isLoaded() && player->isPlaying() && player->getSpeed() > 0.0;
}

void ofxTLVideoTrack::update(ofEventArgs& args){
    
	if(!isLoaded()){
		return;
	}
   	
	if(timeline->getTimecontrolTrack()){
		timeline->setCurrentFrame(player->getCurrentFrame());
	}
	
   	if(timeline->getTimecontrolTrack() == this && isPlaying()){
        
        //this will happen if the user calls play on the video itself
		if(!currentlyPlaying){
            play(); //to trigger events
        }
        
        if(player->getCurrentFrame() < inFrame || player->getCurrentFrame() > outFrame){
            //cout << "reset in frame from " << player->getCurrentFrame() << endl;
            player->setFrame(inFrame);
            //cout << "	to: " << player->getCurrentFrame() << endl;
        }
        
        if(selectedFrame > player->getCurrentFrame()){
            currentLoop++;
            //cout << "LOOPED! with last frame " << lastFrame << " " << player->getCurrentFrame() << " current loop " << currentLoop << endl;
        }

        //if(timeline->getInOutRange().max < player->getPosition()){
        if(timeline->getOutFrame() <= player->getCurrentFrame()){
            if(timeline->getLoopType() == OF_LOOP_NONE){
                stop();                
            }
            else{
                int loopFrame = timeline->getInFrame();
				selectFrame(loopFrame);
                ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
                ofNotifyEvent(events().playbackLooped, args);                
            }
        }
        //else if(timeline->getInOutRange().min > player->getPosition()){
        if(timeline->getInFrame() > player->getCurrentFrame()){
            int loopFrame = timeline->getInFrame();
            selectFrame(loopFrame);
        }
        
        timeline->setPercentComplete(player->getPosition());
        selectedFrame = player->getCurrentFrame();
        
        /*
        //cout << " is playing player frame " << player->getCurrentFrame() << " current frame " << getCurrentFrame() << endl;
		if(timeline->getIsFrameBased()){
            
		}
		else{
			if(timeline->getOutTime() < player->getPosition()*player->getDuration() || timeline->getInTime() > player->getPosition()*player->getDuration() ){
				player->setFrame(timeline->getInOutRange().min * player->getTotalNumFrames());
			}
			timeline->setCurrentTime( player->getPosition() * player->getDuration());
		}
        */

        player->update();
	}
    else{
        if(player->getSpeed() != 0){
	        player->setSpeed(0);
        }
        if(currentlyPlaying){
            stop();
        }
    }
}

void ofxTLVideoTrack::playheadScrubbed(ofxTLPlaybackEventArgs& args){
	
    if(isLoaded() && !currentlyPlaying){
        selectFrame(args.currentFrame);

//        cout << "after scrub timeline time is " << timeline->getCurrentTime()  << " frame is " << timeline->getCurrentFrame() << " and percent is " << timeline->getPercentComplete() << endl;
//        cout << "while video is " << player->getPosition()*player->getDuration() << " frame is " << player->getCurrentFrame() << " and percent is " << player->getPosition() << endl;
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
        if(!ofGetMousePressed() && !currentlyZooming && thumbsEnabled && backthreadedPlayer != NULL && backthreadedPlayer->isLoaded()){
            for(int i = 0; i < backThumbs.size(); i++){
                if(!backThumbs[i].loaded){
                    if(currentlyZooming || ofGetMousePressed()){
                        break;
                    }
                    backthreadedPlayer->setFrame(backThumbs[i].framenum);			

                    if(currentlyZooming || ofGetMousePressed()){
                        break;
                    }
					backThumbs[i].useTexture = false;
                    backThumbs[i].create(backthreadedPlayer->getPixelsRef());
                    
                    lock();
                    videoThumbs[i] = backThumbs[i];
                    unlock();

                }
            }
        }
        backLock.unlock();
     
        ofSleepMillis(100);
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
        backLock.lock();
        backthreadedPlayer = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
        backthreadedPlayer->setUseTexture(false);
        backthreadedPlayer->loadMovie(player->getMoviePath());
        backLock.unlock();
        
		inFrame = 0;
		outFrame = player->getTotalNumFrames();
        
        player->play();
        player->setSpeed(0.);
        player->setFrame(0);
        player->update();
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
	//inFrame = outFrame = -1;
    inFrame = 0;
    outFrame = player->getTotalNumFrames();

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
            ofSetColor(timeline->getColors().textColor);
            ofDrawBitmapString(ofToString(videoThumbs[i].framenum), videoThumbs[i].displayRect.x+5, videoThumbs[i].displayRect.y+15);
            ofRect(videoThumbs[i].displayRect);
		}		
	}
	
    //TODO: draw prettier
	int selectedFrameX = screenXForIndex(selectedFrame);
	ofPushStyle();
	ofFill();
	ofSetColor(timeline->getColors().backgroundColor, 250);
	ofRect(selectedFrameX, bounds.y, 150, bounds.height);
	ofPopStyle();
	ofSetColor(timeline->getColors().textColor);
	ofLine(selectedFrameX, bounds.y, selectedFrameX, bounds.y+bounds.height);
	ofDrawBitmapString("frame " + ofToString(selectedFrame), selectedFrameX, bounds.y+15);
	ofDrawBitmapString("seconds " + ofToString(player->getPosition()*player->getDuration()), selectedFrameX, bounds.y+30);
	
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

void ofxTLVideoTrack::mousePressed(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mousePressed(args, millis);
	if(getDrawRect().inside(args.x, args.y)){
		timeline->unselectAll();
		selectFrame( indexForScreenX(args.x) );

		if(timeline->getMovePlayheadOnDrag()){
			timeline->setPercentComplete(screenXtoNormalizedX(args.x));
		}
	}
}

void ofxTLVideoTrack::mouseMoved(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mouseMoved(args, millis);
}

void ofxTLVideoTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	if(bounds.inside(args.x, args.y)){
		selectFrame( indexForScreenX(args.x) );
		if(timeline->getMovePlayheadOnDrag()){
			timeline->setPercentComplete(screenXtoNormalizedX(args.x));
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


void ofxTLVideoTrack::keyPressed(ofKeyEventArgs& args){
	if(isLoaded() && hasFocus()){
		if(args.key == OF_KEY_LEFT){
			selectFrame(MAX(selectedFrame-1, 0));
		}
		else if(args.key == OF_KEY_RIGHT){
			selectFrame(MIN(selectedFrame+1, player->getTotalNumFrames()-1));		
		}
	}	
}

int ofxTLVideoTrack::selectFrame(int frame){
	selectedFrame = inFrame + (frame % (outFrame - inFrame));

	currentLoop = frame / (outFrame-inFrame);
//	cout << "selecting frame " << selectedFrame << endl;
	player->setFrame(selectedFrame);
	player->update();
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
	//TODO: account for in and out frame.
	return player->getPosition() * player->getDuration();
}

int ofxTLVideoTrack::getSelectedFrame(){
	return selectedFrame + currentLoop * (outFrame-inFrame);
}

void ofxTLVideoTrack::exit(ofEventArgs& args){
    stopThread();
//	waitForThread(true);
}

string ofxTLVideoTrack::getTrackType(){
    return "VideoTrack";
}
