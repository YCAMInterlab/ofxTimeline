/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
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
 */

#pragma once

#include "ofMain.h"

//For lack of a type abstraction, this let's you #define a font renderer before including ofxTimeline
//(like ofxFTGL or ofxFont)
//to use ofxFTGL use somethinglike this:
//#define OFX_TIMELINE_FONT_RENDERER ofxFTGLFont
//#define OFX_TIMELINE_FONT_INCLUDE "ofxFTGLFont.h"

#ifndef OFX_TIMELINE_FONT_RENDERER
#define OFX_TIMELINE_FONT_RENDERER ofTrueTypeFont
#endif

#ifdef OFX_TIMELINE_FONT_INCLUDE
#include OFX_TIMELINE_FONT_INCLUDE
#endif

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
#include "ofxTLCameraTrack.h"
#include "ofxTLColors.h"
#include "ofxTLLFO.h"

#ifdef TIMELINE_VIDEO_INCLUDED
#include "ofxTLVideoTrack.h"
#endif

#ifdef TIMELINE_AUDIO_INCLUDED
#include "ofxTLAudioTrack.h"
#endif


typedef struct {
    ofxTLTrack* track;
    string stateBuffer;
} UndoItem;

class ofxTimeline : ofThread {
  public:
	
	//needed for hotkeys to work
	//optionally pass in an "app name" for Quit.
	static void removeCocoaMenusFromGlut(string appName);
	
	ofxTimeline();
	virtual ~ofxTimeline();

	virtual void setup();
	
	//Optionally run ofxTimeline on the background thread
	//this isn't necessary most of the time but
	//for precise timing apps and input recording it'll greatly
	//improve performance
	virtual void moveToThread();
    virtual void removeFromThread();
	
	bool toggleEnabled();
    void enable();
	void disable();
    bool getIsEnabled();
	
	void enableEvents();
	void disableEvents();

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
    
    virtual bool getSpacebarTogglesPlay();
    virtual void setSpacebarTogglePlay(bool spacebarPlays);
    
	virtual void playSelectedTrack();
	virtual void stopSelectedTrack();
	virtual bool togglePlaySelectedTrack();
	
    virtual void setLoopType(ofLoopType newType);
	virtual ofLoopType getLoopType();
    bool isDone(); //returns true if percentComplete == 1.0 and loop type is none

	virtual bool toggleShow();    
    virtual void show();
	virtual void hide();
	virtual bool getIsShowing();
	virtual void draw();
    
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	virtual void keyPressed(ofKeyEventArgs& args);
	virtual void keyReleased(ofKeyEventArgs& args);
	virtual void windowResized(ofResizeEventArgs& args);
	virtual void exit(ofEventArgs& args);
	
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
    void loadTracksFromFolder(string folderPath);
    void saveTracksToFolder(string folderPath);

    void setDefaultFontPath(string fontPath);
    
    //timing setup functions
    void setFrameRate(float fps);    
    void setDurationInFrames(int frames);
	void setDurationInSeconds(float seconds);
	void setDurationInMillis(unsigned long long millis);
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
    virtual void setCurrentTimeMillis(unsigned long long millis);
	virtual void setPercentComplete(float percent);
	virtual void setCurrentTimecode(string timecodeString);
    
    virtual void setCurrentTimeToInPoint();
	virtual void setCurrentTimeToOutPoint();

	virtual int getCurrentFrame();
    virtual int getCurrentPageIndex();
    virtual string getCurrentPageName();
	virtual float getCurrentTime();
	virtual long getCurrentTimeMillis();
    virtual float getPercentComplete();
	virtual string getCurrentTimecode();
	virtual long getQuantizedTime(unsigned long long time, unsigned long long step);
    
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
    void setInPointAtMillis(unsigned long long millis);
    void setInPointAtTimecode(string timecode);
	void setInPointAtPlayhead();
    
	virtual void setOutPointAtPercent(float percent);
	void setOutPointAtFrame(float frame);
	void setOutPointAtSeconds(float time);
	void setOutPointAtMillis(unsigned long long millis);
    void setOutPointAtTimecode(string timecode);
    void setOutPointAtPlayhead();
    
	virtual void setEditableHeaders(bool headersEditable);
	virtual bool areHeadersEditable();
	
	virtual void setMinimalHeaders(bool headersMinimal);
	virtual bool areHeadersMinimal();
	
	virtual bool toggleShowFooters();
	virtual void setFootersHidden(bool footersHidden);
	virtual bool areFootersHidden();
	
	virtual void setInOutRange(ofRange inoutPercentRange);
	virtual void setInOutRangeMillis(unsigned long long min, unsigned long long max);
	virtual void clearInOut();
    
    ofRange getInOutRange();
	ofLongRange getInOutRangeMillis();
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
	virtual void setHeight(float height);
	virtual void collapseAllTracks(); //collapses all tracks heights to 0;
	
	ofRectangle getDrawRect();
	float getWidth();
	float getHeight();
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
	vector<string>& getPasteboard();
	
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
    
    //TODO: focus track on specific pages
    ofxTLTrack* getFocusedTrack();
    void setFocusedTrack(ofxTLTrack* track);
    
	//Returns the number of items selected in the whole timeline
    //	Subclass Note
    //	ofxTLTracks that allow for multiple selection
    //	should use this value to change the behavior for creating new items
    //	If a user clicks on a blank part of a track it should only
    //	create a new keyframe if 1 or 0 keys is selected
    //	if 2 or more is selected, it just trigger an unselect all before
    //	create any new items
    int getTotalSelectedItems();
	unsigned long long getEarliestTime();
	unsigned long long getLatestTime();
	unsigned long long getEarliestSelectedTime();
	unsigned long long getLatestSelectedTime();

	bool hasTrack(string trackName);
    bool hasPage(string pageName);
    
	//type can be
	//Bangs, Switches, Flags, Colors, Curves, Audio or Video
	ofxTLTrack* getTrack(string name);
	ofxTLPage* getPage(string pageName);
	
	//adding tracks always adds to the current page
    ofxTLCurves* addCurves(string name, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	ofxTLCurves* addCurves(string name, string xmlFileName, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	float getValue(string name);
	float getValueAtPercent(string name, float atPercent);
	float getValue(string name, float atTime);
	float getValue(string name, int atFrame);

	//adding tracks always adds to the current page
    ofxTLLFO* addLFO(string name, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);
	ofxTLLFO* addLFO(string name, string xmlFileName, ofRange valueRange = ofRange(0,1.0), float defaultValue = 0);

    ofxTLSwitches* addSwitches(string name);
	ofxTLSwitches* addSwitches(string name, string xmlFileName);
	bool isSwitchOn(string name);
	bool isSwitchOn(string name, float atTime);
	bool isSwitchOn(string name, int atFrame);
	
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
	
	ofColor getColor(string name);
	ofColor getColorAtPercent(string name, float percent);
	ofColor getColorAtSecond(string name, float second);
	ofColor getColorAtMillis(string name, unsigned long long millis);
    
	void setDefaultColorPalettePath(string path);
	string getDefaultColorPalettePath();
    
    //TODO: remove image sequence from the core? ... or fix it up.
	//*IMAGE SEQUENCE DOES NOT WORK*
	ofxTLImageSequence* addImageSequence(string name);
	ofxTLImageSequence* addImageSequence(string name, string directory);
	ofImage* getImage(string name);
	ofImage* getImage(string name, float atTime);
	ofImage* getImage(string name, int atFrame);
	//*IMAGE SEQUENCE DOES NOT WORK*

	#ifdef TIMELINE_VIDEO_INCLUDED
	ofxTLVideoTrack* addVideoTrack(string name);
    ofxTLVideoTrack* addVideoTrackWithPath(string videoPath);
    ofxTLVideoTrack* addVideoTrack(string name, string videoPath);
    ofxTLVideoTrack* getVideoTrack(string videoTrackName);
    ofPtr<ofVideoPlayer> getVideoPlayer(string videoTrackName);
	#endif
    
	#ifdef TIMELINE_AUDIO_INCLUDED
    //Audio tracks only work with PCM Wav or Aiff file
    ofxTLAudioTrack* addAudioTrack(string trackName);
    ofxTLAudioTrack* addAudioTrackWithPath(string audioPath);
    ofxTLAudioTrack* addAudioTrack(string name, string audioPath);
    ofxTLAudioTrack* getAudioTrack(string audioTrackName);
	#endif

    //used for audio and video.
    //we punt to the track to control time.
    //this can be a video or audio track
    void setTimecontrolTrack(string trackName);
    void setTimecontrolTrack(ofxTLTrack* track);
	ofxTLTrack* getTimecontrolTrack();
    
	//you can add custom tracks this way
	virtual void addTrack(string name, ofxTLTrack* track);

    ofxTLTrackHeader* getTrackHeader(string trackName);
    ofxTLTrackHeader* getTrackHeader(ofxTLTrack* track);
    
    void removeTrack(string name);
    void removeTrack(ofxTLTrack* track);
    
    //ordering the track
    void bringTrackToTop(string name);
    void bringTrackToTop(ofxTLTrack* track);
    void bringTrackToBottom(string name);
    void bringTrackToBottom(ofxTLTrack* track);
	
	void setupFont();
	void setupFont(string newFontPath, int newFontSize);
	OFX_TIMELINE_FONT_RENDERER & getFont();
	
	ofxTLColors& getColors();
	ofxTimecode& getTimecode();
	ofxMSATimer& getTimer();
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
	void setDragTimeOffset(unsigned long long millisecondOffset);
    void cancelSnapping();
	long getDragTimeOffset();
    void setHoverTime(unsigned long long millisTime);
        
    string formatTime(float seconds);
    string formatTime(unsigned long long millis);

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
    ofxTLTrack* timeControl;

    //can be blank, default save to bin/data/
    string workingFolder; 
    
	bool isSetup;
	bool usingEvents;
	bool isOnThread;

	//called when the name changes to setup the inout track, zoomer, ticker etc
	void setupStandardElements();
	
	bool movePlayheadOnPaste;
    long dragMillsecondOffset;
	bool dragAnchorSet; // will disable snapping if no drag anchor is set on mousedown
	bool lockWidthToWindow;
    
	//one string per track
	vector<string> pasteboard;
    
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
    
    bool spacebarTogglesPlay;
    
	float width;
	ofVec2f offset;

	int fontSize;
	string fontPath;
	OFX_TIMELINE_FONT_RENDERER font;
	
	//only enabled while playing
	virtual void update(ofEventArgs& updateArgs);
	virtual void updateTime();
    virtual void threadedFunction(); //only fired after moveToThread()
	virtual void checkLoop();
	virtual void checkEvents();
	

	virtual void viewWasResized(ofEventArgs& args);
	virtual void pageChanged(ofxTLPageEventArgs& args);

	virtual void updatePagePositions();
	virtual void recalculateBoundingRects();
    
	
	string defaultPalettePath;
	
    //TODO convert to ofLongRange
	ofRange inoutRange;
    void triggerInOutEvent();
    
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
	bool headersAreEditable;
	bool minimalHeaders;
	bool footersHidden;
	
	bool isFrameBased;
	float durationInSeconds;
};
