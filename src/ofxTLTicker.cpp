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
	framerate(25),
	curHoverFrame(-1)
{
	//default constructor
}

ofxTLTicker::~ofxTLTicker(){
}

void ofxTLTicker::setup(){
	//empty
}

void ofxTLTicker::draw(){
	
	ofPushStyle();
	
	if(isFrameBased){
		//draw tickers with frame numbers
		float d = bounds.width/(float)durationInFrames; //using a float results in uneven spread of keyframes
		ofSetColor(200, 180, 40);
		int counter = 0;
		for (float i = bounds.x; i < bounds.width; i+=d){
			ofLine(i, bounds.y, i, bounds.y+bounds.height);
			if(counter%10 == 0){
				ofSetLineWidth(3);
				ofLine(i, bounds.y+bounds.height*.85, i, bounds.height);
				ofSetLineWidth(1);
			}
			counter++;
		}

		//highlite current mouse position
		if(curHoverFrame > -1){
			float pos = bounds.x+d*curHoverFrame;
			ofRect(pos, bounds.y, d, bounds.height);
			ofSetColor(0);
			string text = ofToString(curHoverFrame);
			int textH = 10;
			int textW = (text.size()+1)*7;
			ofRect(pos+d, bounds.y+bounds.height-textH, textW, textH);
			ofSetColor(200, 180, 40);
			ofDrawBitmapString(text, pos+d+2, bounds.y+bounds.height);
			
			ofEnableAlphaBlending();
			//draw playhead line
			ofSetColor(255, 0, 0, 150);
			ofSetLineWidth(1);
			ofLine(pos+d, 0, pos+d, totalDrawRect.height);
		}
	}
	else {
		//draw tickers with time
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
	
void ofxTLTicker::mouseDragged(ofMouseEventArgs& args){
	updateHover(args);
	//TODO update time, trigger an event?
}

void ofxTLTicker::mouseMoved(ofMouseEventArgs& args){
	updateHover(args);
}

void ofxTLTicker::mousePressed(ofMouseEventArgs& args){

}

void ofxTLTicker::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLTicker::setTotalDrawRect(ofRectangle drawRect){
	totalDrawRect = drawRect;
}


void ofxTLTicker::updateHover(ofMouseEventArgs& args){
	ofVec2f mousePos(args.x - totalDrawRect.x, args.y - totalDrawRect.y); //necessary or are mouse positions already mapped to the right region?
	if(mousePos.y < 0 || mousePos.y > totalDrawRect.height){
		curHoverFrame = -1;
		return;
	}
	if(isFrameBased){
		curHoverFrame = floorf(mousePos.x/(bounds.width/(float)durationInFrames));
	}
	else{

	}
}
