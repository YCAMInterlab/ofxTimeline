/*
 *  ofxTLZoomer.h
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofRange.h"
#include "ofxTLElement.h"

class ofxTLZoomer : public ofxTLElement
{
  public:
	ofxTLZoomer();
	~ofxTLZoomer();
	
	void draw();
		
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
	void keyPressed(ofKeyEventArgs& args);
	
	virtual void load();
	virtual void save();

	bool isActive();
	
	ofRange getViewRange();
	
  private:

	ofRange currentViewRange;
	
	float minGrabOffset;
	float maxGrabOffset;
	
	bool mouseIsDown;
	bool minSelected;
	bool maxSelected;
	bool midSelected;
	
//	bool pointInScreenBounds(ofVec2f screenpoint);
//	float screenXtoRangeX(float x);
//	float rangeXtoScreenX(float x);
};