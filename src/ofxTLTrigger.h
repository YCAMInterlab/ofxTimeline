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
#include "ofxTextInputField.h"

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

	void playbackStarted(ofxTLPlaybackEventArgs& args);
	void playbackLooped(ofxTLPlaybackEventArgs& args);
	void playbackEnded(ofxTLPlaybackEventArgs& args);
	
  protected:
	ofxTextInputField textfield;
	
	void update(ofEventArgs& args);
	void sortTriggers();
						   
	int dragOffset;
	Trigger* getTriggerForScreenPosition(float screenx, int& offset);
	Trigger* selectedTrigger;
	Trigger* hoverTrigger;
	vector<Trigger> triggers;
	bool enterText;
	
	float lastTimelinePoint;
};