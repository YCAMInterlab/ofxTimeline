/*
 *  ofxTLTicker.h
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"

class ofxTLTicker : public ofxTLElement
{
  public:
	ofxTLTicker();
	~ofxTLTicker();
	
	virtual void draw();
	
	virtual void setDuration(int frames);
	virtual void setDuration(float seconds);
	virtual void setFrameRate(int framerate);
	
  protected:
	bool isFrameBased;
	int durationInFrames;
	float durationInSeconds;
	int framerate;

};