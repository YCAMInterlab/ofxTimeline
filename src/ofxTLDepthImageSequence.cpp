/*
 *  ofxTLDepthImageSequence.cpp
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLDepthImageSequence.h"

ofxTLDepthImageSequence::ofxTLDepthImageSequence(){
	sequenceLoaded = false;
	currentDepthRaw = NULL;
	selectedFrame = 0;
	thumbsEnabled = true;
}

ofxTLDepthImageSequence::~ofxTLDepthImageSequence(){
	if(currentDepthRaw != NULL){
		delete currentDepthRaw;
	}
}

void ofxTLDepthImageSequence::setup(){
	
	enable();
	currentDepthRaw = new unsigned short[640*480];
	
}

void ofxTLDepthImageSequence::draw(){

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
			if(!thumbsEnabled){
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
	
	int selectedFrameX = screenXForIndex(selectedFrame, videoThumbs.size());
	ofSetColor(0, 125, 255);
	ofLine(selectedFrameX, bounds.y, selectedFrameX, bounds.y+bounds.height);
	ofDrawBitmapString(ofToString(selectedFrame), selectedFrameX, bounds.y+35);
	
	ofPopStyle();
}

void ofxTLDepthImageSequence::zoomStarted(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomStarted(args);
	calculateFramePositions();
}

void ofxTLDepthImageSequence::zoomDragged(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomDragged(args);
	calculateFramePositions();
}

void ofxTLDepthImageSequence::zoomEnded(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomEnded(args);
	calculateFramePositions();
	if(thumbsEnabled){
		generateVideoThumbnails();	
	}
}

void ofxTLDepthImageSequence::mousePressed(ofMouseEventArgs& args){
	ofxTLElement::mousePressed(args);
}

void ofxTLDepthImageSequence::mouseMoved(ofMouseEventArgs& args){
	ofxTLElement::mouseMoved(args);
}

void ofxTLDepthImageSequence::mouseDragged(ofMouseEventArgs& args){
	if( bounds.inside(args.x, args.y) ){
		selectFrame( indexForScreenX(args.x, videoThumbs.size()) );
	}
}

void ofxTLDepthImageSequence::keyPressed(ofKeyEventArgs& args){
	if(hover){
		if(args.key == OF_KEY_LEFT){
			selectFrame(MAX(selectedFrame-1, 0));
		}
		else if(args.key == OF_KEY_RIGHT){
			selectFrame(MIN(selectedFrame+1, videoThumbs.size()-1));		
		}
	}
}

void ofxTLDepthImageSequence::selectFrame(int frame){
	selectedFrame = ofClamp(frame, 0, videoThumbs.size()-1);
	decoder.readDepthFrame(videoThumbs[selectedFrame].sourcepath, currentDepthRaw);
	currentDepthImage = decoder.convertTo8BitImage(currentDepthRaw);
}

void ofxTLDepthImageSequence::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLDepthImageSequence::drawRectChanged(){
	calculateFramePositions();
}

void ofxTLDepthImageSequence::loadSequence(){
	ofFileDialogResult r = ofSystemLoadDialog("Load Depth Sequence Directory", true);
	if(r.bSuccess){
		loadSequence(r.getPath());
	}
}

void ofxTLDepthImageSequence::loadSequence(string seqdir){
	ofDirectory sequenceList(seqdir);
	if(!sequenceList.exists()){
		ofLogError("ofxTLDepthImageSequence -- sequence directory " + seqdir + " does not exist!");
		return;
	}
	
	thumbDirectory = seqdir + "/thumbs";
	ofDirectory thumdir(thumbDirectory);
	if(!thumdir.exists()){
		if(!thumdir.create(true)){
			ofLogError("ofxTLDepthImageSequence -- sequence directory " + seqdir + " cannot be created!");
			return;
		}
	}
	
	sequenceList.allowExt("xkcd");
	int numFiles = sequenceList.listDir();
	for(int i = 0; i < numFiles; i++){
		ofxTLVideoThumb	t;
		t.setup(i, thumbDirectory);
		t.sourcepath = sequenceList.getPath(i);
		
		videoThumbs.push_back(t);
	}
	
	sequenceLoaded = true;
	
	videoThumbs[0].visible = true;
	generateThumbnailForFrame(0);
	calculateFramePositions();
	generateVideoThumbnails();
	
}

void ofxTLDepthImageSequence::calculateFramePositions(){
	
	if(!sequenceLoaded){
		return;
	}
	
	int frameWidth = int( bounds.height * videoThumbs[0].targetWidth / videoThumbs[0].targetHeight );
	int totalPixels = int( bounds.width / zoomBounds.span() );
	int framesToShow = MAX(totalPixels / frameWidth, 1);
	int frameStep = MAX(videoThumbs.size() / framesToShow, 1); 
	int minPixelIndex = -(zoomBounds.min * totalPixels);
	
	cout << "bounds are " << bounds.width << " " << bounds.height << " frameWidth " << frameWidth << " total pixels " << totalPixels << " frame step " << frameStep << " minpix " << minPixelIndex << endl;
	
	for(int i = 0; i < videoThumbs.size(); i++){
		if(i % frameStep == 0){
			int screenX = screenXForIndex(i, videoThumbs.size());
			videoThumbs[i].displayRect = ofRectangle(screenX, bounds.y, frameWidth, bounds.height);
			videoThumbs[i].visible = videoThumbs[i].displayRect.x+videoThumbs[i].displayRect.width > 0 && videoThumbs[i].displayRect.x < bounds.width;
		}
		else {
			videoThumbs[i].visible = false;
		}
	}
}

void ofxTLDepthImageSequence::generateVideoThumbnails() {
	for(int i = 0; i < videoThumbs.size(); i++){
		generateThumbnailForFrame(i);
	}
}

void ofxTLDepthImageSequence::generateThumbnailForFrame(int i){
	if(videoThumbs[i].visible && !videoThumbs[i].loaded){
		if(videoThumbs[i].exists){
//			cout << "Loading existing thumb " << i << endl;
			videoThumbs[i].load();
		}
		else {
			ofImage grayConverted = decoder.readDepthFrametoImage(videoThumbs[i].sourcepath);
			videoThumbs[i].create(grayConverted);
		}
	}
}

void ofxTLDepthImageSequence::toggleThumbs(){
	thumbsEnabled = !thumbsEnabled;
}

int ofxTLDepthImageSequence::getSelectedFrame(){
	return selectedFrame;
}
