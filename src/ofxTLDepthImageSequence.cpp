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

}

ofxTLDepthImageSequence::~ofxTLDepthImageSequence(){

}

void ofxTLDepthImageSequence::setup(){
	enable();
}

void ofxTLDepthImageSequence::draw(){
	
}

void ofxTLDepthImageSequence::zoomStarted(ofxTLZoomEventArgs& args){

}

void ofxTLDepthImageSequence::zoomDragged(ofxTLZoomEventArgs& args){

}

void ofxTLDepthImageSequence::zoomEnded(ofxTLZoomEventArgs& args){

}

void ofxTLDepthImageSequence::drawRectChanged(){
	
}

void ofxTLDepthImageSequence::loadDepthSequence(){
	ofFileDialogResult r = ofSystemLoadDialog("Load Depth Sequence Directory", true);
	if(r.bSuccess){
		loadDepthSequence(r.getPath());
	}
}

void ofxTLDepthImageSequence::loadDepthSequence(string seqdir){
	ofDirectory sequenceList(seqdir);
	if(!sequenceList.exists()){
		ofLogError("ofxTLDepthImageSequence -- sequence directory " + seqdir + " does not exist!");
		return;
	}
	   
	
}
