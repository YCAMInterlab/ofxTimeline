/*
 *  ofxTLTrigger.h
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "ofxTLElement.h"

typedef struct {
	float pt;
	string name;
} Trigger;

class ofxTLTrigger : public ofxTLElement
{
  public:
	ofxTLTrigger();
	~ofxTLTrigger();
	
	virtual void setup();
	virtual void draw();
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void save();
	virtual void load();
	
	virtual void clear();
		
  protected:
	int dragOffset;
	Trigger* getTriggerForScreenPosition(float screenx, int& offset);
	Trigger* selectedTrigger;
	Trigger* hoverTrigger;
	vector<Trigger> triggers;
	bool enterText;
};