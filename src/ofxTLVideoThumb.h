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
	~ofxTLVideoThumb();
	
//	void setup(int framenum, string thumbpath);
	void create(ofPixels& thumbPixels);

//	void load();
	void unload();
	
//	string sourcepath;
//	string filepath;
	
	int framenum;
    long timestamp; //generally millis from the beginning of the sequence

	bool loaded;
//  bool pixelsReady;
//	bool exists;

    bool useTexture;
	ofPtr<ofImage> thumb;
	
//	bool visible;
	ofRectangle displayRect;
	
//	float targetWidth;
//	float targetHeight;
//	float timeloaded;
};