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

//TODO: Add global undo
//TODO: Account for image sequences and movies with different durations than the timeline

#pragma once

#include "ofMain.h"

//external addons
#include "ofRange.h"
#include "ofxMSATimer.h"
#include "ofxTimecode.h"

//internal types
#include "ofxTLTrack.h"
#include "ofxTLTrackHeader.h"
#include "ofxTLPage.h"
#include "ofxTLPageTabs.h"
#include "ofxTLZoomer.h"
#include "ofxTLTicker.h"
#include "ofxTLKeyframer.h"
#include "ofxTLTrigger.h"
#include "ofxTLSwitcher.h"
#include "ofxTLImageSequence.h"
#include "ofxTLVideoTrack.h"
#include "ofxTLColors.h"

class ofxTimeline {
  public:
	ofxTimeline();
	~ofxTimeline();

	virtual void setup();
	
	virtual void enable();
	virtual void disable();
	virtual bool toggleEnabled();

    virtual void reset(); //clears every element
    
    virtual void loadElementsFromFolder(string folderPath);
    
	virtual void show();
	virtual void hide();
	virtual bool toggleShow();
	virtual void save();
    
	virtual void setCurrentFrame(int currentFrame);
	virtual void setCurrentTime(float time);
	virtual void setPercentComplete(float percent);
	
	virtual int getCurrentFrame();
	virtual float getCurrentTime();
	virtual float getPercentComplete();
	
    virtual void flagUserChangedValue(); //internal elements call this when the value has changed
    virtual bool getUserChangedValue(); //this returns and clears the flag, generally call once per frame
    
	virtual void play();
	virtual void stop();
	virtual bool togglePlay();
	virtual bool getIsPlaying();
    
	virtual void setInPointAtPlayhead();
    virtual void setOutPointAtPlayhead();
	virtual void setInPointAtPercent(float percent);
	virtual void setOutPointAtPercent(float percent);
	virtual void setInPointAtFrame(int frame);
	virtual void setOutPointAtFrame(float frame);
	virtual void setInPointAtTime(float time);
	virtual void setOutPointAtTime(float time);

	virtual void setInOutRange(ofRange inoutPercentRange);
	virtual ofRange getInOutRange();
	virtual int getInFrame();
	virtual int getOutFrame();
	virtual float getInTime();
	virtual float getOutTime();

	virtual void setCurrentTimeToInPoint();
	virtual void setCurrentTimeToOutPoint();
	
    void setFrameRate(int fps);    
    void setDurationInFrames(int frames);
	void setDurationInSeconds(float seconds);

    //frame based mode timelines will never skip frames, and will advance at the speed of openFrameworks
    //regardless of the FPS that you set the timeline to
    void setFrameBased(bool frameBased);
	bool getIsFrameBased();
        
	int getDurationInFrames();
	float getDurationInSeconds();
	string getDurationInTimecode();

    virtual void setLoopType(ofLoopType newType);
	virtual ofLoopType getLoopType();

	virtual void setOffset(ofVec2f offset);
	virtual void setWidth(float width);
	virtual void collapseAllElements(); //collapses all element heights to 0;
	
	ofRectangle getDrawRect();
	
	void setSnapping(bool snapping);
	void toggleSnapping();
	
	//setting a BPM allows for a global measure across the timeline
	//this is useful for snapping to intervals 
	void enableSnapToBPM(float bpm); //beats per minute
	void toggleDrawBPMGrid();
	void enableDrawBPMGrid(bool enableGrid);
	void enableSnapToOtherElements(bool enableSnapToOther);
	
	void setMovePlayheadOnPaste(bool move);
	bool getMovePlayheadOnPaste();	
	string getPasteboard();
	
	void setMovePlayheadOnDrag(bool updatePlayhead);
	bool getMovePlayheadOnDrag();
	
	void unselectAll();

	virtual void draw();

	virtual void addPage(string name, bool makeCurrent = true);
	virtual void setPageName(string newName);
	virtual void setCurrentPage(string name);
	virtual void setCurrentPage(int number);
	
	virtual ofxTLTrack* getElement(string name);
	
	//adding elements always adds to the current page
    virtual ofxTLKeyframer* addKeyframes(string name, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	virtual ofxTLKeyframer* addKeyframes(string name, string xmlFileName, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
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

    //TODO: remove image sequence from the core?
	virtual ofxTLImageSequence* addImageSequence(string name);
	virtual ofxTLImageSequence* addImageSequence(string name, string directory);
	virtual ofImage* getImage(string name);
	virtual ofImage* getImage(string name, float atTime);
	virtual ofImage* getImage(string name, int atFrame);
	
	
	//for custom elements
	virtual void addElement(string name, ofxTLTrack* element);
	virtual void setAutosave(bool autosave);
		
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	virtual void keyPressed(ofKeyEventArgs& args);
	virtual void windowResized(ofResizeEventArgs& args);

	ofxTLColors& getColors();
	ofxTimecode& getTimecode();

	ofVec2f getNudgePercent();
	ofVec2f getBigNudgePercent();

	//do not call this yourself, called from within TLElement
	//This is a bit subtle why it's here
	//when multi keys are being dragged and snapping is enabled
	//we need to compensate by the drag offset between the grabbed key
	//and the mouse.  
	//TL elements should call this on mousedown if one of their elements is
	//should be snapped directly to snap lines
	void setDragAnchor(float dragAnchor);
	float getDragAnchor();
	
    string formatTime(float time);
    
	ofxTLPlaybackEventArgs createPlaybackEvent();
	
  protected:
	
    ofxMSATimer timer;
    ofxTimecode timecode;
    
	bool isSetup;
	bool usingEvents;
	bool snappingEnabled;
	bool movePlayheadOnPaste;
	float globalDragAnchor;
	bool dragAnchorSet; // will disable snapping if no drag anchor is set on mousedown
	
	string pasteboard;

	bool movePlayheadOnDrag;
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

	virtual void viewWasResized(ofEventArgs& args);
	virtual void pageChanged(ofxTLPageEventArgs& args);

	virtual void updatePagePositions();
	virtual void recalculateBoundingRects();
	
	ofRange inoutRange;
	
    bool timelineHasFocus;
    
    ofxXmlSettings settings;
	string filenamePrefix;
	
	ofRectangle totalDrawRect;
	bool isEnabled; //allows for editing
	bool isShowing; //allows for viewing
	bool isPlaying; //moves playhead along
	bool userChangedValue; //did value change this frame;
    
	float currentTime;
	
	ofLoopType loopType;
	int playbackStartFrame;
	double playbackStartTime;	
	float mouseoverPlayheadPosition;
	float playbackPlayheadPosition;
	
	bool modalIsShown;
	bool autosave;
	
	bool isFrameBased;
	float durationInSeconds;
};
