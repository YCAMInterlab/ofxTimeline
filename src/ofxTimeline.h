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
#include "ofxTLPage.h"
#include "ofxTLPageTabs.h"
#include "ofxTLZoomer.h"
#include "ofxTLTicker.h"
#include "ofxTLKeyframer.h"
#include "ofxTLTrigger.h"
#include "ofxTLSwitcher.h"

class ofxTimeline {
  public:
	ofxTimeline();
	~ofxTimeline();

	virtual void setup();

	virtual void toggleShow();
	virtual void togglePlay();
	
	virtual void enable();
	virtual void disable();

	virtual void setOffset(ofVec2f offset);
	virtual void setWidth(float width);
//	virtual void setTickerHeight(float newTickerHeight);
//	virtual void setZoomerHeight(float newZoomerHeight);
										
	virtual void setDuration(int frames);
	virtual void setDuration(float seconds);
	virtual void setFrameRate(int framerate);

	virtual void draw();

	virtual void addPage(string name, bool makeCurrent = true);
	virtual void setCurrentPage(string name);
	virtual void setCurrentPage(int number);
	
	//adding elements always adds to the current page
	virtual ofxTLKeyframer* addKeyframes(string name, string xmlFileName, ofRange valueRange);
	virtual float getKeyframeValue(string name); 
	virtual float getKeyframeValue(string name, float atTime);
	virtual float getKeyframeValue(string name, int atFrame);
		
	virtual ofxTLSwitcher* addSwitcher(string name, string xmlFileName);
	virtual bool getSwitcherOn(string name);
	virtual bool getSwitcherOn(string name, float atTime);
	virtual bool getSwitcherOn(string name, int atFrame);
	
	virtual ofxTLTrigger* addTriggers(string name, string xmlFileName);
	virtual string getNextTrigger(string name);
	virtual string getNextTrigger(string name, float atTime);
	virtual string getNextTrigger(string name, int atFrame);

	virtual string getLastTrigger(string name);
	virtual string getLastTrigger(string name, float atTime);
	virtual string getLastTrigger(string name, int atFrame);

	//for custom elements
	virtual void addTimelineElement(string name, ofxTLElement* element);
	
	virtual void setAutosave(bool autosave);

  protected:
	
	ofxTLPage* currentPage;
	vector<ofxTLPage*> pages;
	
	ofxTLTicker* ticker;
	ofxTLPageTabs* tabs;
	ofxTLZoomer* zoomer;

	float width;
	ofVec2f offset;
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void windowResized(ofResizeEventArgs& args);
	virtual void viewNeedsResize(ofEventArgs& args);
	virtual void pageChanged(ofxTLPageEventArgs& args);

	virtual void updatePagePositions();
	virtual void recalculateBoundingRects();
	
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
