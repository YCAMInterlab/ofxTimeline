/**
 * ofxTimeline
 *
 * Copyright (c) 2011-2012 James George
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
 *
 * Developed with support of YCAM InterLab
 */


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
#include "ofxTLCurves.h"
#include "ofxTLBangs.h"
#include "ofxTLFlags.h"
#include "ofxTLSwitches.h"
#include "ofxTLColorTrack.h"
#include "ofxTLImageSequence.h"
#include "ofxTLVideoTrack.h"
#include "ofxTLColors.h"
#include "ofxTLTimeController.h"

typedef struct {
    ofxTLTrack* track;
    string stateBuffer;
} UndoItem;

class ofxTimeline : ofThread {
  public:
	ofxTimeline();
	virtual ~ofxTimeline();

	virtual void setup();
	
	//Optionally run ofxTimeline on the background thread
	//this isn't necessary most of the time but
	//for precise timing apps and input recording it'll greatly
	//improve performance
	virtual void moveToThread();
//    virtual void removeFromThread(); //TODO: implement this
	
	bool toggleEnabled();
    virtual void enable();
	virtual void disable();
    
    virtual void clear(); //clears every track
    virtual void reset(); //gets rid of everything, sets back to one page

    //used mostly for generating unique XML
    //for custom names, call setup() before calling name
    virtual void setName(string name);
    virtual string getName();

    //playback
	virtual void play();
	virtual void stop();
	virtual bool togglePlay();
	virtual bool getIsPlaying();

    virtual void setLoopType(ofLoopType newType);
	virtual ofLoopType getLoopType();
    bool isDone(); //returns true if percentComplete == 1.0 and loop type is none

	virtual bool toggleShow();    
    virtual void show();
	virtual void hide();
	virtual void draw();
//	virtual void draw(ofEventArgs& args);

    //show/hide ticker,zoomer,inout all at once
    virtual void setShowTimeControls(bool shouldShowTimeControls);
    virtual void setShowTicker(bool shouldShowTicker);
    virtual void setShowInoutControl(bool shouldShowInoutControl);
    virtual void setShowZoomer(bool shouldShowZoomer);

    //sets where to save all timeline-related meta data xml files
    virtual void setWorkingFolder(string folderPath);
    virtual string getWorkingFolder();
    
    //loads calls load on all tracks from the given folder
    //really useful for setting up 'project' directories
    virtual void loadTracksFromFolder(string folderPath);
    
    //timing setup functions
    void setFrameRate(float fps);    
    void setDurationInFrames(int frames);
	void setDurationInSeconds(float seconds);
	void setDurationInMillis(long millis);
    void setDurationInTimecode(string timecode);

	int getDurationInFrames();
	float getDurationInSeconds();
    long getDurationInMilliseconds();
	string getDurationInTimecode();

    //frame based mode timelines will never skip frames, and will advance at the speed of openFrameworks
    //regardless of the FPS that you set the timeline to
    void setFrameBased(bool frameBased);
	bool getIsFrameBased();

    //autosave will always write to XML file on each major change 
    //otherwise call save manually to write the files
    void setAutosave(bool autosave);
	virtual void save();
	//if there have been changes without a save.
	//if autosave is on this will always return false
	bool hasUnsavedChanges();
	
	virtual void setCurrentFrame(int currentFrame);
	virtual void setCurrentTimeSeconds(float time);
    virtual void setCurrentTimeMillis(long millis);
	virtual void setPercentComplete(float percent);
	virtual void setCurrentTimecode(string timecodeString);
    
    virtual void setCurrentTimeToInPoint();
	virtual void setCurrentTimeToOutPoint();

	virtual int getCurrentFrame();
	virtual float getCurrentTime();
	virtual long getCurrentTimeMillis();
    virtual float getPercentComplete();
	virtual string getCurrentTimecode();
    
    //internal tracks call this when the value has changed slightly
    //so that views can know if they need to update
    virtual void flagUserChangedValue();
    //this returns and clears the flag, generally call once per frame
    virtual bool getUserChangedValue(); 
    
    //internal tracks call this when a big change has been made and it will trigger a save
    virtual void flagTrackModified(ofxTLTrack* track); 
    
    //in and out point setting, respected by internal time
	virtual void setInPointAtPercent(float percent);
	void setInPointAtFrame(int frame);
	void setInPointAtSeconds(float time);
    void setInPointAtMillis(long millis);
    void setInPointAtTimecode(string timecode);
	void setInPointAtPlayhead();
    
	virtual void setOutPointAtPercent(float percent);
	void setOutPointAtFrame(float frame);
	void setOutPointAtSeconds(float time);
	void setOutPointAtMillis(long millis);
    void setOutPointAtTimecode(string timecode);
    void setOutPointAtPlayhead();
    
	virtual void setInOutRange(ofRange inoutPercentRange);
	virtual void clearInOut();
    
    ofRange getInOutRange();
	int getInFrame();
	float getInTimeInSeconds();
	long getInTimeInMillis();
    string getInPointTimecode();
    
    int getOutFrame();
	float getOutTimeInSeconds();
	long getOutTimeInMillis();
    string getOutPointTimecode();

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
    	
	//setting a BPM allows for a global measure across the timeline
	//this is useful for snapping to intervals
	void setBPM(float bpm);
	float getBPM();
	
    bool toggleSnapToBPM();
    void enableSnapToBPM(bool enableSnap);
    bool getSnapToBPM();
    
	bool toggleShowBPMGrid();
	void setShowBPMGrid(bool enableGrid);
    bool getShowBPMGrid();
    
    bool toggleSnapToOtherKeyframes();
	void enableSnapToOtherKeyframes(bool enableSnapToOther);
	bool getSnapToOtherElements();
    
	void setMovePlayheadOnPaste(bool move);
	bool getMovePlayheadOnPaste();	
	string getPasteboard();
	
    //Undo and Redo
    void enableUndo(bool enabled);
    void undo();
    void redo();
    
	void setMovePlayheadOnDrag(bool updatePlayhead);
	bool getMovePlayheadOnDrag();
	
	void unselectAll();

	virtual void addPage(string name, bool makeCurrent = true);
	virtual void setPageName(string newName);
	virtual void setPageName(string newName, int index);
	virtual void setCurrentPage(string name);
	virtual void setCurrentPage(int number);
	
    //if a timeline is modal it means that it may be presenting
    //content outside of the timeline or requiring keyboard input
    //testApp should respect this value and not trigger other hotkeys
    //while isModal() is true so the user can freely type
    bool isModal();
    ofxTLTrack* getModalTrack();
    
    //Subclass Note
    //ofxTLTracks that allow for multiple selection
    //should use this value to change the behavior for creating new items
    //If a user clicks on a blank part of a track it should only
    //create a new keyframe if 1 or 0 keys is selected
    //if 2 or more is selected, it just trigger an unselect all before
    //create any new items
    
    //Returns the number of items selected in the whole timeline
    int getTotalSelectedItems();
    
	virtual ofxTLTrack* getTrack(string name);
	
	//adding tracks always adds to the current page
    ofxTLCurves* addCurves(string name, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	ofxTLCurves* addCurves(string name, string xmlFileName, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	virtual float getValue(string name);
	virtual float getValue(string name, float atTime);
	virtual float getValue(string name, int atFrame);

    virtual ofxTLSwitches* addSwitches(string name);
	virtual ofxTLSwitches* addSwitches(string name, string xmlFileName);
	virtual bool isSwitchOn(string name);
	virtual bool isSwitchOn(string name, float atTime);
	virtual bool isSwitchOn(string name, int atFrame);
	
    ofxTLBangs* addBangs(string name);
	ofxTLBangs* addBangs(string name, string xmlFileName);
    
    ofxTLFlags* addFlags(string name);
    ofxTLFlags* addFlags(string name, string xmlFileName);

	ofxTLColorTrack* addColors(string name); //adds with the default palette
	ofxTLColorTrack* addColors(string name, string xmlFileName); //adds with the default palette
	ofxTLColorTrack* addColorsWithPalette(string name, ofImage& palette);
	ofxTLColorTrack* addColorsWithPalette(string name, string palettePath);
	ofxTLColorTrack* addColorsWithPalette(string name, string xmlFileName, ofImage& palette);
	ofxTLColorTrack* addColorsWithPalette(string name, string xmlFileName, string palettePath);
	
	virtual ofColor getColor(string name);
	virtual ofColor getColorAtPercent(string name, float percent);
	virtual ofColor getColorAtSecond(string name, float second);
	virtual ofColor getColorAtMillis(string name, unsigned long millis);
	
	string getDefaultColorPalettePath();
    //TODO: remove image sequence from the core? ... or fix it up.
	//*IMAGE SEQUENCE DOES NOT WORK*
	virtual ofxTLImageSequence* addImageSequence(string name);
	virtual ofxTLImageSequence* addImageSequence(string name, string directory);
	virtual ofImage* getImage(string name);
	virtual ofImage* getImage(string name, float atTime);
	virtual ofImage* getImage(string name, int atFrame);
	//*IMAGE SEQUENCE DOES NOT WORK*
	
	virtual ofxTLVideoTrack* addVideoTrack(string trackName);
    virtual ofxTLVideoTrack* addVideoTrack(string name, string videoPath);
    virtual ofxTLVideoTrack* getVideoTrack(string videoTrackName);
    virtual ofPtr<ofVideoPlayer> getVideoPlayer(string videoTrackName);
    
    //used for audio and video.
    //we punt to the track to control time.
    //this can be a video or audio track
    virtual void setTimecontrolTrack(ofxTLTimeController* track);
	virtual ofxTLTimeController* getTimecontrolTrack();
    
	//you can add custom tracks this way
	virtual void addTrack(string name, ofxTLTrack* track);

    virtual ofxTLTrackHeader* getTrackHeader(string trackName);
    virtual ofxTLTrackHeader* getTrackHeader(ofxTLTrack* track);
    
    virtual void removeTrack(string name);
    virtual void removeTrack(ofxTLTrack* track);
    
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
	virtual void keyReleased(ofKeyEventArgs& args);
	virtual void windowResized(ofResizeEventArgs& args);

	ofxTLColors& getColors();
	ofxTimecode& getTimecode();
	ofxTLZoomer* getZoomer();
	
	vector<ofxTLPage*>& getPages();
    
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
    void cancelSnapping();
	long getDragTimeOffset();
    void setHoverTime(long millisTime);
        
    string formatTime(float seconds);
    string formatTime(long millis);

    string nameToXMLName(string name);
    string confirmedUniqueName(string name);

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
    
	//binary test hack
	bool curvesUseBinary;
	
  protected:

    ofxTimecode timecode;
	ofxMSATimer timer;
    ofxTLEvents timelineEvents;
    ofxTLColors colors;

    ofxTLInOut* inoutTrack;
	ofxTLTicker* ticker;
	ofxTLPageTabs* tabs;
	ofxTLZoomer* zoomer;

	vector<ofxTLPage*> pages;
	ofxTLPage* currentPage;
    map<string, ofxTLPage*> trackNameToPage;

    ofxTLTrack* modalTrack;
    ofxTLTimeController* timeControl;

    //can be blank, default save to bin/data/
    string workingFolder; 
    
	bool isOnThread;
	bool isSetup;
	bool usingEvents;

	bool movePlayheadOnPaste;
    long dragMillsecondOffset;
	bool dragAnchorSet; // will disable snapping if no drag anchor is set on mousedown
	bool lockWidthToWindow;
    
	string pasteboard;
    
    bool undoEnabled; //turn off undo if you don't need it, it'll take up a ton of memory
    void collectStateBuffers();
    void pushUndoStack();
    void restoreToState(vector<UndoItem>& state);
    
    //this is populated on mouse-down or key-down with all items that could potentially be modified
	vector<UndoItem> stateBuffers; 
    //then after the events are propagated all the modified tracks are collected here 
    set<ofxTLTrack*> modifiedTracks;
    //finally, the state buffers for the tracks that were modified are pushed onto the stack say that state may be returned
    deque< vector<UndoItem> > undoStack;
    //the undo pointer points into the array and lets the user move through undo/redo actions
    int undoPointer;
    
	bool movePlayheadOnDrag;
    bool snapToBPM;
    bool snapToOtherElements;
    
	float width;
	ofVec2f offset;

	//only enabled while playing
	virtual void update(ofEventArgs& updateArgs);
    virtual void threadedFunction(); //only fired after moveToThread()
	virtual void checkLoop();
	virtual void checkEvents();
	
	virtual void enableEvents();
	virtual void disableEvents();

	virtual void viewWasResized(ofEventArgs& args);
	virtual void pageChanged(ofxTLPageEventArgs& args);

	virtual void updatePagePositions();
	virtual void recalculateBoundingRects();
    
	string defaultPalettePath;
	
    //TODO convert to long
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
    
    //TODO: switch to millis!
	float currentTime; 	
	ofLoopType loopType;
	int playbackStartFrame;
	double playbackStartTime;	

	bool autosave;
	bool unsavedChanges;
	
	bool isFrameBased;
	float durationInSeconds;
};
