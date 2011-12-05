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
#include "ofRange.h"
#include "ofxTLElement.h"

class ofxTLZoomer : public ofxTLElement
{
  public:
	ofxTLZoomer();
	~ofxTLZoomer();
	
	void setup();
	void draw();
		
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
	void keyPressed(ofKeyEventArgs& args);
	
	virtual void load();
	virtual void save();

	bool isActive();
	
	ofRange getViewRange();
	
  private:

	void notifyZoomStarted();
	void notifyZoomDragged(ofRange oldZoom);
	void notifyZoomEnded();

	ofRange currentViewRange;
	
	float minGrabOffset;
	float maxGrabOffset;
	
	bool mouseIsDown;
	bool minSelected;
	bool maxSelected;
	bool midSelected;
	
//	bool pointInScreenBounds(ofVec2f screenpoint);
//	float screenXtoRangeX(float x);
//	float rangeXtoScreenX(float x);
};