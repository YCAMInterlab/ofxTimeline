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
#include "ofxXmlSettings.h"
#include "ofxTween.h"
#include "ofRange.h"
#include "ofxTLEvents.h"

class ofxTimeline; 
class ofxTLTrack 
{
  public:
	ofxTLTrack();
	~ofxTLTrack();
	
	virtual void setup();
    void _draw(); //calls draw() internally plus some universal stuff
    //override this in your sublcass
	virtual void draw(){};
    
    //draw modal content is called after the main draw() call
    //override this if you want to draw content that can show up on top of other timelines
	//useful in conjunction with timeline->presentedModalContent(this)
    //for showing big controls, ie the tweener window
	virtual void drawModalContent(){}; 
	
	virtual void enable();
	virtual void disable();
	virtual bool isEnabled();
    
    virtual bool isActive();
	virtual bool hasFocus();
	
    //managed by the page object, do not set yourself
	virtual void setDrawRect(ofRectangle bounds);
	virtual void offsetDrawRect(ofVec2f offset);
	float getBottomEdge();
    
    //returns the screenspace position of the elements bounds, not including header and footer
	virtual ofRectangle getDrawRect();
	
    //saving and loading
    virtual string getXMLFilePath();
    virtual string getXMLFileName();
    
	virtual void setXMLFileName(string filename);
	
    //parent wrappers that call virtual versions implemented by subclasses
    void _mousePressed(ofMouseEventArgs& args);
    void _mouseMoved(ofMouseEventArgs& args);
    void _mouseReleased(ofMouseEventArgs& args);
    
	//standard events to be implement in subclasses
	virtual void mousePressed(ofMouseEventArgs& args){}
	virtual void mouseMoved(ofMouseEventArgs& args){}
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped){};
	virtual void mouseReleased(ofMouseEventArgs& args){};
	
	virtual void keyPressed(ofKeyEventArgs& args){};
	virtual void nudgeBy(ofVec2f nudgePercent){};
	
    //Triggered by the page object based on user interaction, Only calls when the focus actually changes
    virtual void gainedFocus();
    virtual void lostFocus();
    
    //This is triggered when a shift+click+drag rectangle finish on the timeline
    //it passes normalized 0-1 ranges for time (x) and value (y)
    //It's the responsability of the ofxTLTrack to respond appropriately
    //and select the relevant elements under the track
    virtual void regionSelected(ofRange timeRange, ofRange valueRange){};
    
	//copy+paste
	virtual string copyRequest(){return "";};
	virtual string cutRequest(){return "";};
	virtual void pasteSent(string pasteboard){};
	virtual void selectAll(){};
	virtual void unselectAll(){};
	
	//zoom events
	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
    //managed by the zoom call backs and initialized by the Page upon creation
	void setZoomBounds(ofRange zoomBoundsPercent);

	virtual void save(){};
	virtual void load(){};
	virtual void clear(){};
	
	//add any points (in screenspace x) that should be snapped to
	virtual void getSnappingPoints(vector<float>& points){};
	
	ofxTimeline* getTimeline();
	//set by the timeline it's self, no need to call this yourself
	void setTimeline(ofxTimeline* timeline); 
	string getName();
	void setName(string name);
		
	bool getCreatedByTimeline();
	void setCreatedByTimeline(bool created);
	ofxTLEvents& events(); //convenience wrapper for timeline events;
    
  protected:
	
	ofxTimeline* timeline;
	bool enabled; 

	virtual bool pointInScreenBounds(ofVec2f screenpoint);
	virtual float screenXtoNormalizedX(float x);
	virtual float normalizedXtoScreenX(float x);

	virtual float screenXtoNormalizedX(float x, ofRange outputRange);
	virtual float normalizedXtoScreenX(float x, ofRange inputRange);

    //TODO: move this to the main timline
	virtual int indexForScreenX(int mouseX);
	virtual int screenXForIndex(int index);
	virtual int indexForScreenX(int screenX, int durationInFrames);
	virtual int screenXForIndex(int index, int durationInFrames);
	
	virtual float screenXForTime(float time);
	virtual float timeForScreenX(float screenX);
	virtual float screenXForTime(float time, float durationInSeconds);
	virtual float timeForScreenX(float screenX, float durationInSeconds);
		
	virtual void drawRectChanged(){};
	
	virtual bool isOnScreen(float screenX);
	
	bool hover; //mouse is over the element
    bool active; //mouse is clicking on the element
	bool focused; //this element was the last one interacted with
    
	bool createdByTimeline;

	string name;
	string xmlFileName;
	ofRectangle bounds;
	ofRange zoomBounds;	
};
