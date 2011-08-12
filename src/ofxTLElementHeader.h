/*
 *  ofxTLElementHeader.h
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "ofxTLElement.h"

class ofxTLElementHeader : public ofxTLElement
{
  public:	
	ofxTLElementHeader();
	~ofxTLElementHeader();
	string name;	
	
	virtual void setup();
	virtual void draw();
		
};

