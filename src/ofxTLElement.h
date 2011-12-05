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
class ofxTLElement 
{
  public:
	ofxTLElement();
	~ofxTLElement();
	
	virtual void setup() = 0;
	virtual void draw() = 0;
	
	virtual void enable();
	virtual void disable();
	
	virtual bool hasFocus();
	
	virtual void setDrawRect(ofRectangle bounds);
	virtual void offsetDrawRect(ofVec2f offset);
	
	virtual ofRectangle getDrawRect();
	
	virtual void setZoomBounds(ofRange zoomBoundsPercent); //allows you to zoom in!
	
	virtual void setXMLFileName(string filename);
	virtual void setAutosave(bool autosave);
	
	//standard events
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args){};
	virtual void mouseReleased(ofMouseEventArgs& args){};
	
	virtual void keyPressed(ofKeyEventArgs& args){};
	
	//zoom events
	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	virtual void save(){};
	virtual void load(){};
	
	virtual void reset(){};
	virtual void clear(){};
	
	
	ofxTimeline* getTimeline();
	//set by the timeline it's self, no need to call this yourself
	void setTimeline(ofxTimeline* timeline); 
	string getName();
	void setName(string name);
		
	bool getCreatedByTimeline();
	void setCreatedByTimeline(bool created);
	
  protected:
	
	ofxTimeline* timeline;

	virtual bool pointInScreenBounds(ofVec2f screenpoint);
	virtual float screenXtoNormalizedX(float x);
	virtual float normalizedXtoScreenX(float x);

	virtual float screenXtoNormalizedX(float x, ofRange outputRange);
	virtual float normalizedXtoScreenX(float x, ofRange inputRange);

	virtual int indexForScreenX(int mouseX);
	virtual int screenXForIndex(int index);
	virtual int indexForScreenX(int screenX, int durationInFrames);
	virtual int screenXForIndex(int index, int durationInFrames);
	
	virtual void drawRectChanged(){};
	
	bool hover;
	bool focused;
	bool enabled; //it's up to the implementation to respect this
	bool autosave; //it's up to the implementation to save when it's modified based on this var
	
	bool createdByTimeline;
	
	ofxXmlSettings settings;
	string name;
	string xmlFileName;
	ofRectangle bounds;
	ofRange zoomBounds;
	
};
