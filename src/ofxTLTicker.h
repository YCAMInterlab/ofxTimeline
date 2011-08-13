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
	
	virtual void setup();
	virtual void draw();
	
	virtual void setDuration(int frames);
	virtual void setDuration(float seconds);
	virtual void setFrameRate(int framerate);

	//set the draw rect for the whole keyframer interface
	void setTotalDrawRect(ofRectangle drawRect);
	
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);

protected:
	void updateHover(ofMouseEventArgs& args);
	
	ofRectangle totalDrawRect;
	
	bool isFrameBased;
	int durationInFrames;
	float durationInSeconds;
	int framerate;
	
	int curHoverFrame;
	float curHoverTime;
	float curHoverPoint; //normalized
};
