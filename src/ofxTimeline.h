/*
 *  THISTimeline.h
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 */


//controls to add:
// ofxTLFilmstrip
// ofxTLGradient

//JG current priorities:
//connect zoomer through events
//add 

//STAGE 1
//TODO: Add playback
//TODO: Add loop modes
//TODO: Add pages
//TOOD: Add movable headers
//TODO: Add built-in zoomer
//TODO: Add built-in frame markers 
//TODO: Add built-in playhead
//TODO: Add timing functions and play
//TODO: Add modality
//TODO: Add collapsable members
//TODO: Add remove keyframe options

//TODO: Add headers swap location when dragged across one another

//TODO: Create events for modal
//TODO: Add snap to bottom
//TODO: Add variable width (instead of using full width)
//TODO: Add global undo
//TODO: Add changable colors

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofxTLElementHeader.h"
#include "ofxTLZoomer.h"
#include "ofxTLTicker.h"
#include "ofxTLKeyframer.h"
#include "ofxTLTrigger.h"
#include "ofxTLSwitcher.h"

class ofxTimeline 
{
  public:
	ofxTimeline();
	~ofxTimeline();

	void setup();

	void toggleShow();
	void togglePlay();
	
	void enable();
	void disable();

	void setDuration(int frames);
	void setDuration(float seconds);
	void setFrameRate(int framerate);

	void draw();
	
	ofxTLKeyframer* addKeyframes(string name, string xmlFileName, ofRange valueRange);
	float getKeyframeValue(string name); 
	float getKeyframeValue(string name, float atTime);
	float getKeyframeValue(string name, int atFrame);
		
	ofxTLSwitcher* addSwitcher(string name, string xmlFileName);
	bool getSwitcherOn(string name);
	bool getSwitcherOn(string name, float atTime);
	bool getSwitcherOn(string name, int atFrame);
	
	ofxTLTrigger* addTriggers(string name, string xmlFileName);
	string getNextTrigger(string name);
	string getNextTrigger(string name, float atTime);
	string getNextTrigger(string name, int atFrame);

	string getLastTrigger(string name);
	string getLastTrigger(string name, float atTime);
	string getLastTrigger(string name, int atFrame);

	//for custom elements
	void addTimelineElement(string name, ofxTLElement* element);
	
	void setAutosave(bool autosave);

  private:
	vector<ofxTLElementHeader*> headers;
	map<string, ofxTLElement*> elements;
	ofxTLTicker* ticker;
	ofxTLZoomer* zoomer;

	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
	void keyPressed(ofKeyEventArgs& args);
	
	void windowResized(ofResizeEventArgs& args);
	
	void viewNeedsResize(ofEventArgs& args);
						 
	void recalculateBoundingRects();
	
    ofxXmlSettings settings;
	string filenamePrefix;
	
	ofRectangle bounds;
	bool isEnabled; //allows for editing
	bool isShowing; //allows for viewing
	bool isPlaying; //moves playhead along
	
	int playbackStartFrame;
	float playbackStartTime;	
	float mouseoverPlayheadPosition;
	float playbackPlayheadPosition;
	
	bool modalIsShown;
	bool autosave;
	bool isFrameBased;
	float durationInFrames;
	float durationInSeconds;
	int currentFrameRate;
};
