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
    useTexture = true;
    timestamp = 0;
}

ofxTLVideoThumb::~ofxTLVideoThumb(){
	unload();
}

void ofxTLVideoThumb::create(ofPixels& thumbPixels){

	if(thumb == NULL){
        thumb = ofPtr<ofImage>(new ofImage());
    }
    
    thumb->setUseTexture(useTexture);
	thumb->setFromPixels(thumbPixels);
    
	if(int(displayRect.height) != int(thumbPixels.getHeight()) ){
		thumb->resize(int(displayRect.width), int(displayRect.height));
    }
	loaded = true;
}

void ofxTLVideoThumb::unload(){
	loaded = false;
}