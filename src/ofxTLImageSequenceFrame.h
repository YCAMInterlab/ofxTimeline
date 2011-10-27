/*
 *  ofxTLImageSequenceFrame.h
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

class ofxTLImageSequenceFrame
{
  public:
	ofxTLImageSequenceFrame();
	~ofxTLImageSequenceFrame();
	
	string filename;
    string thumbFilename;
    string shortFilename;
	float lastUsedTime;
	
	void setType(ofImageType type);
	
	void setFrame(string filename);
	ofImage* getFrame();
	ofImage* getThumbnail();
	
	void setDesiredThumbnailWidth(int width);
	
    int getFullFrameWidth();
    int getFullFrameHeight();
	
    int getThumbWidth();
    int getThumbHeight();
	
    bool loadThumb();
	bool loadFrame();
    bool isFrameLoaded();
    bool isThumbLoaded();
	
	void clear();
	
protected:
	int desiredThumbWidth;
	
    int frameWidth;
    int frameHeight;
    int thumbWidth;
    int thumbHeight;
	
	bool frameLoaded;
	bool thumbLoaded;
	
	ofImage* frame;
	ofImage* thumbnail;
	ofImageType type;	
};
