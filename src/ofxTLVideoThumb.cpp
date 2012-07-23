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
//	exists = false;
//s	targetWidth = 360;	
}

ofxTLVideoThumb::~ofxTLVideoThumb(){
	unload();
}

void ofxTLVideoThumb::setup(int frame, string thumbpath){
	framenum = frame;
	char fp[1024];
	sprintf(fp, "%s/thumb_%05d.png", thumbpath.c_str(), framenum);
	filepath = string(fp);
//	exists = ofFile(filepath).exists();
	loaded = false;
}

void ofxTLVideoThumb::create(ofPixels& thumbPixels){

//	if(exists){
//		//let's not create something that already exists
//		return;
//	}
    
	if(thumb == NULL){
        thumb = ofPtr<ofImage>(new ofImage());
    }
    
    
    thumb->setUseTexture(false);
	thumb->setFromPixels(thumbPixels);
//	targetHeight = float(thumbPixels.getHeight()) / thumbPixels.getWidth() * targetWidth;
	if(int(displayRect.height) != int(thumbPixels.getHeight()) ){
		thumb->resize(int(displayRect.width), int(displayRect.height));
    }
//	thumb->saveImage(filepath);	
//	exists = true;
	loaded = true;
}

//void ofxTLVideoThumb::load(){
////	timeloaded = ofGetElapsedTimef();
//	if(!exists){
//		ofLogError("ofxTLVideoThumb -- Trying to load " + filepath + " but hasn't been created first");
//		return;
//	}
//	
//	if(!thumb->loadImage(filepath)){
//		ofLogError("ofxTLVideoThumb -- Trying to load " + filepath + " but hasn't been created first");
//		return;
//	}
//	
//	targetWidth = thumb->getWidth();
//	targetHeight = thumb->getHeight(); 
//	loaded = true;
//}

void ofxTLVideoThumb::unload(){
	loaded = false;
}