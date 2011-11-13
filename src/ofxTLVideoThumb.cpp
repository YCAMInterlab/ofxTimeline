/*
 *  ofxTLVideoThumb.cpp
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLVideoThumb.h"

ofxTLVideoThumb::ofxTLVideoThumb(){
	framenum= -1;
	loaded = false;
	exists = false;
	targetWidth = 360;	
}

ofxTLVideoThumb::~ofxTLVideoThumb(){

}

void ofxTLVideoThumb::setup(int framenum, string thumbpath){
	char fp[1024];
	sprintf(fp, "%s/thum_%05d.png", thumbpath.c_str(), framenum);
	filepath = string(fp);
	exists = ofFile(filepath).exists();
	loaded = false;
}

void ofxTLVideoThumb::create(ofImage& videoFrame){

	if(exists){
		//let's not create something that already exists
		return;
	}
	
	thumb.clone(videoFrame);
	int targetHeight = videoFrame.getWidth()/targetWidth * videoFrame.getHeight();
	thumb.resize(targetWidth, targetHeight);
	thumb.saveImage(filepath);
	exists = true;
	loaded = true;
}

void ofxTLVideoThumb::load(){
	timeloaded = ofGetElapsedTimef();
	if(!exists){
		ofLogError("ofxTLVideoThumb -- Trying to load " + filepath + " but hasn't been created first");
		return;
	}
	
	thumb.loadImage(filepath);
}

void ofxTLVideoThumb::unload(){
	thumb.clear();
	loaded = false;
}