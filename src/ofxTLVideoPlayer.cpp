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
}

ofxTLVideoPlayer::~ofxTLVideoPlayer(){

}

void ofxTLVideoPlayer::setup(){
	enable();
}

void ofxTLVideoPlayer::draw(){
	if(player == NULL){
		return;
	}
	
	if(player->isPlaying()){
		if(timeline->getIsFrameBased()){
			timeline->stop();
			timeline->setCurrentFrame( player->getCurrentFrame() );
		}
		else {
			timeline->setCurrentTime( player->getPosition() * player->getDuration());
		}
	}
	//if we were scrubbing we need to reset the player's current frame
	else if(selectedFrame != player->getCurrentFrame()){
		player->setFrame(selectedFrame);
	}
	player->update();
	   
	ofPushStyle();
	if(thumbsEnabled){
		ofSetColor(255);
		for(int i = 0; i < videoThumbs.size(); i++){
			if(videoThumbs[i].visible){
				videoThumbs[i].thumb.draw(videoThumbs[i].displayRect);
			}
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
	
	int selectedFrameX = screenXForIndex(selectedFrame);
	ofSetColor(0, 125, 255);
	ofLine(selectedFrameX, bounds.y, selectedFrameX, bounds.y+bounds.height);
	ofDrawBitmapString(ofToString(selectedFrame), selectedFrameX, bounds.y+35);
	
	ofPopStyle();
}


void ofxTLVideoPlayer::setVideoPlayer(ofVideoPlayer& newPlayer, string thumbDir){
	ofDirectory checkCreateDirectory(thumbDir);
	if (!checkCreateDirectory.exists()) {
		ofLogError("ofxTLVideoPlayer -- Directory " + thumbDir + " doesn't exist! Creating");
		checkCreateDirectory.create(true);
	}
	
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
	
	int frameWidth = int( bounds.height * videoThumbs[0].targetWidth / videoThumbs[0].targetHeight );
	int totalPixels = int( bounds.width / zoomBounds.span() );
	int framesToShow = MAX(totalPixels / frameWidth, 1);
	int frameStep = MAX(videoThumbs.size() / framesToShow, 1); 
	int minPixelIndex = -(zoomBounds.min * totalPixels);

	//cout << "bounds are " << bounds.width << " " << bounds.height << " frameWidth " << frameWidth << " total pixels " << totalPixels << " frame step " << frameStep << " minpix " << minPixelIndex << endl;
	
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
}

void ofxTLVideoPlayer::mouseMoved(ofMouseEventArgs& args){
	ofxTLElement::mouseMoved(args);
}

void ofxTLVideoPlayer::mouseDragged(ofMouseEventArgs& args){
	if(bounds.inside(args.x, args.y)){
		selectFrame( indexForScreenX(args.x) );
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

void ofxTLVideoPlayer::selectFrame(int frame){
	selectedFrame = ofClamp(frame, 0, videoThumbs.size()-1);
	player->setFrame(selectedFrame);
	player->update();
}

void ofxTLVideoPlayer::generateVideoThumbnails(){
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


int ofxTLVideoPlayer::getSelectedFrame(){
	return selectedFrame;
}

void ofxTLVideoPlayer::purgeOldThumbnails(){
	for(int i = 0; i < videoThumbs.size(); i++){
		//TODO
	}
}

