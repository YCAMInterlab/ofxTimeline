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
#include "ofxTLTweener.h"
#include "ofxTLBangs.h"
#include "ofxTLFlags.h"
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
    
    //timing setup functions
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
    bool isDone(); //returns true if percentComplete == 1.0 and loop type is none
    
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
	
    //internal elements call this when the value has changed slightly
    //so that views can know if they need to update
    virtual void flagUserChangedValue();
    //this returns and clears the flag, generally call once per frame
    virtual bool getUserChangedValue(); 
    
    //internal elements call this when a big change has been made
    //it'll trigger a save if autosave is on, and it'll add to the undo queue for that page
    virtual void flagTrackModified(ofxTLTrack* track); 
    
    //playback
	virtual void play();
	virtual void stop();
	virtual bool togglePlay();
	virtual bool getIsPlaying();
    
    //in and out point setting, respected by internal time
	virtual void setInPointAtPlayhead();
    virtual void setOutPointAtPlayhead();
	virtual void setInPointAtPercent(float percent);
	virtual void setOutPointAtPercent(float percent);
	virtual void setInPointAtFrame(int frame);
	virtual void setOutPointAtFrame(float frame);
	virtual void setInPointAtTime(float time);
	virtual void setOutPointAtTime(float time);

	virtual void setInOutRange(ofRange inoutPercentRange);
	
    ofRange getInOutRange();
	int getInFrame();
	int getOutFrame();
	float getInTime();
	float getOutTime();

	virtual void setCurrentTimeToInPoint();
	virtual void setCurrentTimeToOutPoint();


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
	
    bool isModal();
    ofxTLTrack* getModalTrack();
    
	virtual ofxTLTrack* getElement(string name);
	
	//adding elements always adds to the current page
    ofxTLTweener* addKeyframes(string name, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	ofxTLTweener* addKeyframes(string name, string xmlFileName, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	virtual float getKeyframeValue(string name); 
	virtual float getKeyframeValue(string name, float atTime);
	virtual float getKeyframeValue(string name, int atFrame);

    virtual ofxTLSwitcher* addSwitcher(string name);
	virtual ofxTLSwitcher* addSwitcher(string name, string xmlFileName);
	virtual bool getSwitcherOn(string name);
	virtual bool getSwitcherOn(string name, float atTime);
	virtual bool getSwitcherOn(string name, int atFrame);
	
    ofxTLBangs* addBangs(string name);
	ofxTLBangs* addBangs(string name, string xmlFileName);
    
    ofxTLFlags* addFlags(string name);
    ofxTLFlags* addFlags(string name, string xmlFileName);

    //TODO: remove image sequence from the core?
	virtual ofxTLImageSequence* addImageSequence(string name);
	virtual ofxTLImageSequence* addImageSequence(string name, string directory);
	virtual ofImage* getImage(string name);
	virtual ofImage* getImage(string name, float atTime);
	virtual ofImage* getImage(string name, int atFrame);
	
	//for custom elements
	virtual void addElement(string name, ofxTLTrack* element);
	void setAutosave(bool autosave);
		
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
	
    //when an track calls presentedModalContent all key and mouse action will be sent directly to that element
    //until dismissedModalContent() is called. This will is for displaying custom controls that may overlap with other
    //elements or require keyboard input (see how it's used in ofxTLTweener and ofxTLTrigger)
    void presentedModalContent(ofxTLTrack* modalTrack);
    void dismissedModalContent();
    
    //time <-> pixel translation helpers
    float screenXtoNormalizedX(float x);
    float normalizedXtoScreenX(float x);
    float screenXtoNormalizedX(float x, ofRange outputRange);
    float normalizedXtoScreenX(float x, ofRange inputRange);    
    
  protected:
	
    ofxMSATimer timer;
    ofxTimecode timecode;
    
    string nameToXMLName(string name);
    
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
	map<string, ofxTLPage*> trackNameToPage;
	
	ofxTLPage* currentPage;
	vector<ofxTLPage*> pages;

	ofxTLTicker* ticker;
	ofxTLPageTabs* tabs;
	ofxTLZoomer* zoomer;

    ofxTLTrack* modalTrack;
    
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
