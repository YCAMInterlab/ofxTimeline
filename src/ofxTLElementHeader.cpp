/*
 *  ofxTLElementHeader.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLElementHeader.h"

ofxTLElementHeader::ofxTLElementHeader(){
	
}

ofxTLElementHeader::~ofxTLElementHeader(){

}

void ofxTLElementHeader::setup(){
	enable();
	load();
}

void ofxTLElementHeader::draw(){
	ofPushStyle();
	
	ofSetColor(250, 180, 80);
	
	
	ofNoFill();
	ofDrawBitmapString( name + " header la la la ", ofPoint(bounds.x + 30, bounds.y + 10) );
	ofRect(bounds);
	
	ofPopStyle();
}