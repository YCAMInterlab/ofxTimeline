/*
 *  ofxTLVideoPlayer.cpp
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLVideoPlayer.h"
#include "ofxTimeline.h"

ofxTLVideoPlayer::ofxTLVideoPlayer() {
	player = NULL;	
	thumbsEnabled = true;
	currentLoop = 0;
	inFrame = -1;
	outFrame = -1;
    thumbnailUpdatedWidth = -1;
    thumbnailUpdatedHeight = -1;

}

ofxTLVideoPlayer::~ofxTLVideoPlayer(){

}

void ofxTLVideoPlayer::setup(){
	enable();
    ofAddListener(ofEvents().update, this, &ofxTLVideoPlayer::update);
}

void ofxTLVideoPlayer::update(ofEventArgs& args){
	if(player == NULL){
		return;
	}
	
   	if(player->isPlaying() && player->getSpeed() > 0.0){
		
        //		cout << " is playing player frame " << player->getCurrentFrame() << " current frame " << getCurrentFrame() << endl;
		if(timeline->getIsFrameBased()){
            
			if(player->getCurrentFrame() < inFrame || player->getCurrentFrame() > outFrame){
                //				cout << "reset in frame from " << player->getCurrentFrame() << endl;
				player->setFrame(inFrame);
                //				cout << "	to: " << player->getCurrentFrame() << endl;
			}
			
			if(lastFrame > player->getCurrentFrame()){
				currentLoop++;
                //				cout << "LOOPED! with last frame " << lastFrame << " " << player->getCurrentFrame() << " current loop " << currentLoop << endl;
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

void ofxTLVideoPlayer::draw(){
	
	if(player == NULL){
		return;
	}
	
//	cout << "in out is " << inFrame << " " << outFrame << endl;
	
	ofPushStyle();
	
	if(thumbsEnabled && getDrawRect().height > 10){
        
        if(!ofGetMousePressed() && (thumbnailUpdatedWidth != getDrawRect().width || thumbnailUpdatedHeight != getDrawRect().height) ) {
        	generateVideoThumbnails();	
        }
        
		ofSetColor(255);
		for(int i = 0; i < videoThumbs.size(); i++){
			if(videoThumbs[i].visible){
				videoThumbs[i].thumb.draw(videoThumbs[i].displayRect);
			}
		}
		
		for(int i = 0; i < videoThumbs.size(); i++){
			if(videoThumbs[i].visible){

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
	}
	
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

ofVideoPlayer & ofxTLVideoPlayer::getPlayer(){
	return *player;
}

void ofxTLVideoPlayer::setInFrame(int in){
	inFrame = in;
}

void ofxTLVideoPlayer::setOutFrame(int out){
	outFrame = out;
}

void ofxTLVideoPlayer::setVideoPlayer(ofVideoPlayer& newPlayer, string thumbDir){
	ofDirectory checkCreateDirectory(thumbDir);
	if (!checkCreateDirectory.exists()) {
		ofLogError("ofxTLVideoPlayer -- Directory " + thumbDir + " doesn't exist! Creating");
		checkCreateDirectory.create(true);
	}
	
	if(inFrame == -1){
		cout << "reseting in out" << endl;
		inFrame = 0;
		outFrame = newPlayer.getTotalNumFrames();
	}
	
	//TODO: check out
	videoThumbs.clear();
	player = &newPlayer;
	thumbDirectory = thumbDir;
	for(int i = 0; i < newPlayer.getTotalNumFrames(); i++){
		ofxTLVideoThumb t;
		t.setup(i, thumbDir);
		videoThumbs.push_back(t);
	}
	
	videoThumbs[0].visible = true;
	generateThumbnailForFrame(0);
	calculateFramePositions();
	generateVideoThumbnails();
}


void ofxTLVideoPlayer::drawRectChanged(){
	calculateFramePositions();
}

void ofxTLVideoPlayer::zoomStarted(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomStarted(args);
	calculateFramePositions();
}

void ofxTLVideoPlayer::zoomDragged(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomDragged(args);
	calculateFramePositions();
}

void ofxTLVideoPlayer::zoomEnded(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomEnded(args);
	calculateFramePositions();
	if(thumbsEnabled){
		generateVideoThumbnails();
		purgeOldThumbnails();
	}
}

void ofxTLVideoPlayer::calculateFramePositions(){
	
	if(player == NULL){
		return;
	}
	
    if(bounds.height < 10){
        return;
    }
    
	int frameWidth = int( bounds.height * videoThumbs[0].targetWidth / videoThumbs[0].targetHeight );
	int totalPixels = int( bounds.width / zoomBounds.span() );
	int framesToShow = MAX(totalPixels / frameWidth, 1);
	int frameStep = MAX(videoThumbs.size() / framesToShow, 1); 
	int minPixelIndex = -(zoomBounds.min * totalPixels);

	//cout << "bounds are " << bounds.width << " "f  << bounds.height << " frameWidth " << frameWidth << " total pixels " << totalPixels << " frame step " << frameStep << " minpix " << minPixelIndex << endl;
	
	for(int i = 0; i < videoThumbs.size(); i++){
		if(i % frameStep == 0){
			int screenX = screenXForIndex(i);
			videoThumbs[i].displayRect = ofRectangle(screenX, bounds.y, frameWidth, bounds.height);
			videoThumbs[i].visible = videoThumbs[i].displayRect.x+videoThumbs[i].displayRect.width > 0 && videoThumbs[i].displayRect.x < bounds.width;
		}
		else {
			videoThumbs[i].visible = false;
		}
	}
}

void ofxTLVideoPlayer::mousePressed(ofMouseEventArgs& args){
	ofxTLElement::mousePressed(args);
	if(getDrawRect().inside(args.x, args.y)){
		timeline->unselectAll();
	}
}

void ofxTLVideoPlayer::mouseMoved(ofMouseEventArgs& args){
	ofxTLElement::mouseMoved(args);
}

void ofxTLVideoPlayer::mouseDragged(ofMouseEventArgs& args, bool snapped){
	if(bounds.inside(args.x, args.y)){
		selectFrame( indexForScreenX(args.x) );
		if(timeline->getMovePlayheadOnDrag()){
//			cout << "setting percent complete " << 
			timeline->setPercentComplete(screenXtoNormalizedX(args.x, zoomBounds));
		}
	}
}

void ofxTLVideoPlayer::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLVideoPlayer::keyPressed(ofKeyEventArgs& args){
	if(hover){
		if(args.key == OF_KEY_LEFT){
			selectFrame(MAX(selectedFrame-1, 0));
		}
		else if(args.key == OF_KEY_RIGHT){
			selectFrame(MIN(selectedFrame+1, videoThumbs.size()-1));		
		}
	}	
}

int ofxTLVideoPlayer::selectFrame(int frame){
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

void ofxTLVideoPlayer::generateVideoThumbnails(){
    thumbnailUpdatedWidth  = getDrawRect().width;
    thumbnailUpdatedHeight = getDrawRect().height;
	for(int i = 0; i < videoThumbs.size(); i++){
		generateThumbnailForFrame(i);
	}
}

void ofxTLVideoPlayer::generateThumbnailForFrame(int i){
	if(videoThumbs[i].visible && !videoThumbs[i].loaded){
		if(videoThumbs[i].exists){
			videoThumbs[i].load();
		}
		else {
			player->setFrame(videoThumbs[i].framenum);			
			player->update();

			ofImage frameImage;
			frameImage.setFromPixels(player->getPixelsRef());			
			videoThumbs[i].create(frameImage);
		}
	}
}

void ofxTLVideoPlayer::toggleThumbs(){
	thumbsEnabled = !thumbsEnabled;
}

int ofxTLVideoPlayer::getCurrentFrame(){
	//return player->getCurrentFrame() + currentLoop*player->getTotalNumFrames();
	return (player->getCurrentFrame() - inFrame) + currentLoop*(outFrame-inFrame);
}

float ofxTLVideoPlayer::getCurrentTime(){
	return player->getPosition() * player->getDuration();
}

int ofxTLVideoPlayer::getSelectedFrame(){
	return selectedFrame + currentLoop * (outFrame-inFrame);
}

void ofxTLVideoPlayer::purgeOldThumbnails(){
	for(int i = 0; i < videoThumbs.size(); i++){
		//TODO
	}
}

