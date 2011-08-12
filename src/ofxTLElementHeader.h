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

#define FOOTER_HEIGHT 10

class ofxTLElementHeader : public ofxTLElement
{
  public:	
	ofxTLElementHeader();
	~ofxTLElementHeader();
	
	string name;	
	
	virtual void setup();
	virtual void draw();
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void setElement(ofxTLElement* element);
	
  protected:
	ofxTLElement* element;
	
	bool draggingSize;
	float dragOffset;
	float dragAnchor;
	bool collapsed;
};

