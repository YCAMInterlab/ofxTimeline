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

//STAGE 1
//TODO: Add getting values back
//TODO: Add loop modes
//TODO: Add timing functions and play
//TODO: Add modality
//TODO: Add collapsable members

//TODO: Create events for modal
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
#include "ofxTLImageSequence.h"

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


	virtual ofxTLImageSequence* addImageSequence(string name);
	virtual ofxTLImageSequence* addImageSequence(string name, string directory);
	virtual ofImage* getImage(string name);
	virtual ofImage* getImage(string name, float atTime);
	virtual ofImage* getImage(string name, int atFrame);
	
	
	//for custom elements
	virtual void addTimelineElement(string name, ofxTLElement* element);
	
	virtual void setAutosave(bool autosave);
	
	void enableEvents();
	void disableEvents();
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	virtual void keyPressed(ofKeyEventArgs& args);
	virtual void windowResized(ofResizeEventArgs& args);

  protected:
	bool isSetup;
	bool usingEvents;
	
	ofxTLPage* currentPage;
	vector<ofxTLPage*> pages;
	
	ofxTLTicker* ticker;
	ofxTLPageTabs* tabs;
	ofxTLZoomer* zoomer;

	float width;
	ofVec2f offset;

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
