/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxTimeline 
 * Lightweight SDK for creating graphic timeline tools in openFrameworks
 */

//controls to add:
// ofxTLColorTween
// ofxTLSwitcher

//TODO: Implement time-based side of things in many places
//TODO: Add global undo
//TODO: Add multi-keyframe select
//TODO: Add modal function
//TODO: Account for image sequences and movies with different durations than the timeline
//TODO: Copy+Paste keyframes

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
#include "ofxTLVideoPlayer.h"
#include "ofxTLColors.h"

class ofxTimeline {
  public:
	ofxTimeline();
	~ofxTimeline();

	virtual void setup();
	
	virtual void enable();
	virtual void disable();
	virtual bool toggleEnabled();

	virtual void show();
	virtual void hide();
	virtual bool toggleShow();

	virtual void setCurrentFrame(int currentFrame);
	virtual void setCurrentTime(float time);
	virtual void setPercentComplete(float percent);
	
	virtual int getCurrentFrame();
	virtual float getCurrentTime();
	virtual float getPercentComplete();
	
	virtual void play();
	virtual void stop();
	virtual bool togglePlay();
	virtual bool getIsPlaying();
	
	//Setting the duration in seconds or frames determines 
	//wether the timeline is frame-based or time-based
	//FRAME-BASED timelines will progress one frame each call to update
	//at whatever framerate openFrameworks is running at, and will never skip frames
	//TIME-BASED timelines will progress in real time regardless of openFrameworks
	//framerate.
	virtual void setDurationInFrames(int frames);
	virtual void setDurationInSeconds(float seconds);
	virtual bool getIsFrameBased();
	
	virtual bool setLoopType(ofLoopType newType);
	
	virtual int getDurationInFrames();
	virtual float getDurationInSeconds();

	virtual void setOffset(ofVec2f offset);
	virtual void setWidth(float width);
	
	virtual void draw();

	virtual void addPage(string name, bool makeCurrent = true);
	virtual void setPageName(string newName);
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
	virtual void addElement(string name, ofxTLElement* element);
	virtual void setAutosave(bool autosave);
		
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	virtual void keyPressed(ofKeyEventArgs& args);
	virtual void windowResized(ofResizeEventArgs& args);

	ofxTLColors& getColors();
	
  protected:
	bool isSetup;
	bool usingEvents;
	
	//only enabled while playing
	virtual void update(ofEventArgs& updateArgs);

	ofxTLColors colors;
	map<string, ofxTLPage*> elementNameToPage;
	
	ofxTLPage* currentPage;
	vector<ofxTLPage*> pages;

	ofxTLTicker* ticker;
	ofxTLPageTabs* tabs;
	ofxTLZoomer* zoomer;

	float width;
	ofVec2f offset;

	virtual void enableEvents();
	virtual void disableEvents();

	virtual void viewNeedsResize(ofEventArgs& args);
	virtual void pageChanged(ofxTLPageEventArgs& args);

	virtual void updatePagePositions();
	virtual void recalculateBoundingRects();
	
	ofxTLPlaybackEventArgs createPlaybackEvent();
	
    ofxXmlSettings settings;
	string filenamePrefix;
	
	ofRectangle bounds;
	bool isEnabled; //allows for editing
	bool isShowing; //allows for viewing
	bool isPlaying; //moves playhead along
	
	int currentFrame;
	float currentTime;
	
	ofLoopType loopType;
	int playbackStartFrame;
	float playbackStartTime;	
	float mouseoverPlayheadPosition;
	float playbackPlayheadPosition;
	
	bool modalIsShown;
	bool autosave;
	
	bool isFrameBased;
	int durationInFrames;
	float durationInSeconds;
};
