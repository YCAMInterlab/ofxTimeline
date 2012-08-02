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

#pragma once

#include "ofMain.h"
#include "ofxTLTrack.h"
#include "ofxTLTrackHeader.h"
#include "ofxTLEvents.h"

class ofxTimeline;
class ofxTLTicker;
class ofxTLPage {
  public:
	
	ofxTLPage();
	~ofxTLPage();
	
	virtual void setup();
	virtual void draw();

	virtual void setName(string name);
	virtual string getName();
	
	virtual void setContainer(ofVec2f offset, float width);
	virtual void setHeaderHeight(float newHeaderHeight);
	virtual void setDefaultTrackHeight(float newDefaultTrackHeight);
	
	virtual void collapseAllTracks(); //collapses all track heights to 0;
	
	virtual void setZoomBounds(ofRange zoomBounds);
	
	virtual void unselectAll();
    virtual void clear();
    virtual void save();
    
	virtual float getComputedHeight();	
	virtual float getBottomEdge();
    virtual ofRectangle getDrawRect();
    
	virtual void addTrack(string name, ofxTLTrack* track);
	virtual ofxTLTrack* getTrack(string name);
	virtual void removeTrack(string name);
    
    //given a folder the page will look for xml files to load within that
	virtual void loadTracksFromFolder(string folderPath);
    
	virtual void setSnapping(bool snapping);
	virtual void enableSnapToOtherTracks(bool snapToOthes);
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
		
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
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	
	virtual void setDragAnchor(float anchor);
	
    ofxTimeline* timeline;
    
    
    
  protected:
    
	//used for getting BPM snaps
	vector<ofxTLTrackHeader*> headers;
	map<string, ofxTLTrack*> tracks;

    bool draggingInside;

	ofxTLTicker* ticker;	
    ofxTLTrack* focusedTrack;

	bool snappingEnabled;
	bool snapToOtherTracksEnabled;
	
	bool headerHasFocus;
	
	vector<float> snapPoints;
	float snappingTolerance;
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	void refreshSnapPoints();
	
	float dragAnchor;
	
    bool draggingSelectionRectangle;
    ofVec2f selectionRectangleAnchor;
    ofRectangle selectionRectangle;
    
	bool isSetup;
	string name;
	
	ofRectangle trackContainerRect;
	float headerHeight;
	float defaultTrackHeight;
	ofRange currentZoomBounds;

	map<string, ofRectangle> savedTrackPositions;	
	
};