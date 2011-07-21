/*
 *  ofxTLTicker.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLTicker.h"

ofxTLTicker::ofxTLTicker() 
:	isFrameBased(false),	
	durationInFrames(0),
	durationInSeconds(0),
	framerate(25)
{
	//default constructor
}

ofxTLTicker::~ofxTLTicker(){
}

void ofxTLTicker::draw(){
	
	ofPushStyle();
	
	if(isFrameBased){
		//draw tickers with frame numbers
	}
	else {
		//draw tickers with time
	}
	
	//TEMP
	ofSetColor(200, 180, 40);
	for (int i = bounds.x; i < bounds.width; i+=4){
		ofLine(i, bounds.y, i, bounds.y+bounds.height);
	}
	
	ofNoFill();
	ofSetColor(200, 180, 40);
	ofRect(bounds);
	
	ofPopStyle();
	
}

void ofxTLTicker::setDuration(int frames){
	isFrameBased = true;
	durationInFrames = frames;
	durationInSeconds = 1.0*frames/framerate;
}

void ofxTLTicker::setDuration(float seconds){
	isFrameBased = false;
	durationInSeconds = seconds;
	durationInFrames = seconds*framerate;
}

void ofxTLTicker::setFrameRate(int _framerate){
	framerate = _framerate;
	
	//TODO: maintain duration
	if(isFrameBased){
		
	}
	else{
	}
}
	
