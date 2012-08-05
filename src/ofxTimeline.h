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
#include "ofxTLPage.h"
#include "ofxTLPageTabs.h"
#include "ofxTLZoomer.h"
#include "ofxTLTicker.h"
#include "ofxTLInOut.h"
#include "ofxTLTweener.h"
#include "ofxTLBangs.h"
#include "ofxTLFlags.h"
#include "ofxTLSwitcher.h"
#include "ofxTLImageSequence.h"
#include "ofxTLVideoTrack.h"
#include "ofxTLColors.h"
#include "ofxTLTimeController.h"

class ofxTimeline {
  public:
	ofxTimeline();
	~ofxTimeline();

	virtual void setup();
    
    //used mostly for generating unique XML
    //for custom names, call setup() before calling name
    virtual void setName(string name);
    virtual string getName();
	    
    bool toggleEnabled();
    virtual void enable();
	virtual void disable();
	
    virtual void clear(); //clears every track
    
    //loads calls load on all tracks from the given folder
    //really useful for setting up 'project' directories
    virtual void loadTracksFromFolder(string folderPath);
    
    //timing setup functions
    void setFrameRate(float fps);    
    void setDurationInFrames(int frames);
	void setDurationInSeconds(float seconds);
    
    //frame based mode timelines will never skip frames, and will advance at the speed of openFrameworks
    //regardless of the FPS that you set the timeline to
    void setFrameBased(bool frameBased);
	bool getIsFrameBased();
    
	int getDurationInFrames();
	float getDurationInSeconds();
	string getDurationInTimecode();
    long getDurationInMilliseconds();
        
    virtual void setLoopType(ofLoopType newType);
	virtual ofLoopType getLoopType();
    bool isDone(); //returns true if percentComplete == 1.0 and loop type is none
    
	virtual void show();
	virtual void hide();
	virtual bool toggleShow();
    
    //show/hide ticker,zoomer,inout all at once
    virtual void setShowTimeControls(bool shouldShowTimeControls);
    
    virtual void setShowTicker(bool shouldShowTicker);
    virtual void setShowInoutControl(bool shouldShowInoutControl);
    virtual void setShowZoomer(bool shouldShowZoomer);
    
    //autosave will always write to XML file on each major change 
    //otherwise call save manually to write the files
    void setAutosave(bool autosave);
	virtual void save();

	virtual void setCurrentFrame(int currentFrame);
	virtual void setCurrentTimeSeconds(float time);
    virtual void setCurrentTimeMillis(long millis);
	virtual void setPercentComplete(float percent);
	
	virtual int getCurrentFrame();
	virtual float getCurrentTime();
	virtual long getCurrentTimeMillis();
    virtual float getPercentComplete();
	
    //internal tracks call this when the value has changed slightly
    //so that views can know if they need to update
    virtual void flagUserChangedValue();
    //this returns and clears the flag, generally call once per frame
    virtual bool getUserChangedValue(); 
    
    //internal tracks call this when a big change has been made
    //it'll trigger a save if autosave is on, and it'll add to the undo queue for that page
    virtual void flagTrackModified(ofxTLTrack* track); 
    
    //playback
    //TODO: move these into a time controller of its own
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
    virtual void setLockWidthToWindow(bool lockWidth);
    virtual bool getLockWidthToWindow();
	virtual void setWidth(float width);
	virtual void collapseAllTracks(); //collapses all tracks heights to 0;
	
	ofRectangle getDrawRect();
    ofVec2f getTopRight();
    ofVec2f getTopLeft();
	ofVec2f getBottomLeft();
    ofVec2f getBottomRight();
    
	void setSnapping(bool snapping);
	void toggleSnapping();
	
	//setting a BPM allows for a global measure across the timeline
	//this is useful for snapping to intervals
	void setBPM(float bpm);
	float getBPM();
	void enableSnapToBPM(float bpm); //beats per minute
	void toggleDrawBPMGrid();
	void enableDrawBPMGrid(bool enableGrid);
	void enableSnapToOtherKeyframes(bool enableSnapToOther);
	
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
    
	virtual ofxTLTrack* getTrack(string name);
	
	//adding tracks always adds to the current page
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
	
    virtual ofxTLVideoTrack* addVideoTrack(string name, string videoPath);
    virtual ofxTLVideoTrack* getVideoTrack(string videoTrackName);
    virtual ofPtr<ofVideoPlayer> getVideoPlayer(string videoTrackName);
    
    //used for audio and video.
    //we punt to the track to control time.
    //this can be a video or audio track
    virtual void setTimecontrolTrack(ofxTLTimeController* track);
	virtual ofxTLTimeController* getTimecontrolTrack();
    
	//for custom tracks
	virtual void addTrack(string name, ofxTLTrack* track);

	//TODO: bfd
//   virtual void removeTrack(string name);
    
    //ordering the track
    virtual void bringTrackToTop(string name);
    virtual void bringTrackToTop(ofxTLTrack* track);
    virtual void bringTrackToBottom(string name);
    virtual void bringTrackToBottom(ofxTLTrack* track);
    
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

	//do not call this yourself, called from within TLTrack
	//This is a bit subtle why it's here
	//when multi keys are being dragged and snapping is enabled
	//we need to compensate by the drag offset between the grabbed key
	//and the mouse.  
	//TLTracks should call this on mousedown if one of their tracks is
	//should be snapped directly to snap lines
	void setDragTimeOffset(long millisecondOffset);
	float getDragTimeOffset();
    void setHoverTime(long millisTime);
        
    string formatTime(float seconds);
    string formatTime(long millis);

	ofxTLPlaybackEventArgs createPlaybackEvent();
	
    //when an track calls presentedModalContent all key and mouse action will be sent directly to that tracks
    //until dismissedModalContent() is called. This will is for displaying custom controls that may overlap with other
    //tracks or require keyboard input (see how it's used in ofxTLTweener and ofxTLFlags)
    void presentedModalContent(ofxTLTrack* modalTrack);
    void dismissedModalContent();
    
    //time <-> pixel translation helpers
    long screenXToMillis(float x);
    float millisToScreenX(long millis); 
    float screenXtoNormalizedX(float x);
    float normalizedXtoScreenX(float x);
    float screenXtoNormalizedX(float x, ofRange outputRange);
    float normalizedXtoScreenX(float x, ofRange inputRange);    
    
    virtual ofxTLEvents& events();
    
  protected:
    ofxTimecode timecode;
	ofxMSATimer timer;
    ofxTLEvents timelineEvents;
    
    
    string nameToXMLName(string name);
    
	bool isSetup;
	bool usingEvents;
	bool snappingEnabled;
	bool movePlayheadOnPaste;
//	float globalDragAnchor;
    long dragMillsecondOffset;
	bool dragAnchorSet; // will disable snapping if no drag anchor is set on mousedown
	bool lockWidthToWindow;
    
	string pasteboard;

	bool movePlayheadOnDrag;
	//only enabled while playing
	virtual void update(ofEventArgs& updateArgs);

	ofxTLColors colors;
	map<string, ofxTLPage*> trackNameToPage;
	
	ofxTLPage* currentPage;
	vector<ofxTLPage*> pages;

    ofxTLInOut* inoutTrack;
	ofxTLTicker* ticker;
	ofxTLPageTabs* tabs;
	ofxTLZoomer* zoomer;

    ofxTLTrack* modalTrack;
    ofxTLTimeController* timeControl;
    
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
    
    bool showTicker; 
    bool showInoutControl; 
    bool showZoomer;
    
    ofxXmlSettings settings;
	string name;
	
	ofRectangle totalDrawRect;
	bool isEnabled; //allows for editing
	bool isShowing; //allows for viewing
	bool isPlaying; //moves playhead along
	bool userChangedValue; //did value change this frame;
    
	float currentTime; //TODO: switch to millis!
	
	ofLoopType loopType;
	int playbackStartFrame;
	double playbackStartTime;	

	bool autosave;
	
	bool isFrameBased;
	float durationInSeconds;
};
