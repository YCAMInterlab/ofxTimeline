/*
 *  ofxTLVideoThumb.h
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

class ofxTLVideoThumb {
  public:
	ofxTLVideoThumb();
	virtual ~ofxTLVideoThumb();
	
	void create(ofPixels& thumbPixels);
    
	void unload();
	
	int framenum;
    long timestamp; //generally millis from the beginning of the sequence
	bool loaded;

    bool useTexture;
	ofPtr<ofImage> thumb;
	ofRectangle displayRect;
};