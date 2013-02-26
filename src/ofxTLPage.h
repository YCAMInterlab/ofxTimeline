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
#include "ofxTLTrack.h"
#include "ofxTLTrackHeader.h"
#include "ofxTLEvents.h"

//typedef struct {
//	long millis;
//    float screenX;
//} ofxTLSnapPoint;

class ofxTimeline;
class ofxTLTicker;
class ofxTLPage {
  public:
	
	ofxTLPage();
	virtual ~ofxTLPage();
	
	virtual void setup();
	virtual void update();
	virtual void draw();

	virtual void setName(string name);
	virtual string getName();
	
	
	virtual void setContainer(ofVec2f offset, float width);
	virtual void setHeaderHeight(float newHeaderHeight);
	virtual void setDefaultTrackHeight(float newDefaultTrackHeight);
	
	void setMinimalHeaders(bool minimal);
	void hideFooters(bool hide);
	//collapses all track heights to 0;
	void collapseAllTracks(bool excludeFocusTrack = false);
	//evenly distributes all tracks, restoring the height if it was just collapsed
	void evenlyDistributeTrackHeights();
	void expandFocusedTrack();
	void setExpandToHeight(float height);
		
	virtual void setZoomBounds(ofRange zoomBounds);
	
	virtual void unselectAll();
    virtual void clear();
    virtual void save();

    virtual ofxTLTrack* getFocusedTrack();
	
	virtual float getComputedHeight();	
	virtual float getBottomEdge();
    virtual ofRectangle getDrawRect();
    
	virtual void addTrack(string name, ofxTLTrack* track);
	virtual ofxTLTrack* getTrack(string name);
    virtual ofxTLTrackHeader* getTrackHeader(ofxTLTrack* track);
    
	virtual void removeTrack(ofxTLTrack* track);

    
    //computed on the fly so please use sparingly if you have to call it a lot
    vector<ofxTLTrack*>& getTracks();

    //given a folder the page will look for xml files to load within that
	virtual void loadTracksFromFolder(string folderPath);
    
	virtual void mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
		
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void saveTrackPositions();
	virtual void loadTrackPositions();
	
	virtual void recalculateHeight();
	
	virtual void setTicker(ofxTLTicker* ticker);
	
    virtual void timelineGainedFocus();
    virtual void timelineLostFocus();
    
    void bringTrackToTop(ofxTLTrack* track);
    void bringTrackToBottom(ofxTLTrack* track);

	//copy paste
	virtual void copyRequest(vector<string>& bufs);
	virtual void cutRequest(vector<string>& bufs);
	virtual void pasteSent(const vector<string>& pasteboard);
	virtual void selectAll();
	
	virtual void setDragOffsetTime(long offsetMillis);
    virtual void setSnappingEnabled(bool enabled);
    
    ofxTimeline* timeline;
    
  protected:

	//used for getting BPM snaps
	vector<ofxTLTrackHeader*> headers;
	map<string, ofxTLTrack*> tracks;
	vector<ofxTLTrack*> trackList;
	
    ofxTLTicker* ticker;
    ofxTLTrack* focusedTrack;

	string name;

    bool isSetup;
    bool draggingInside;
	bool headerHasFocus;
	bool footerIsDragging;
	bool snappingEnabled;
	
	set<unsigned long long> snapPoints; //in millis
	float snappingTolerance; //in pixels
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	void refreshSnapPoints();
	
	long millisecondDragOffset;
	
	bool headersAreMinimal;
	bool footersAreHidden;
    bool draggingSelectionRectangle;
    ofVec2f selectionRectangleAnchor;
    ofRectangle selectionRectangle;
    
	float heightBeforeCollapse;
	ofRectangle trackContainerRect;
	float headerHeight;
	float defaultTrackHeight;
	ofRange currentZoomBounds;

	map<string, ofRectangle> savedTrackPositions;	
	
};