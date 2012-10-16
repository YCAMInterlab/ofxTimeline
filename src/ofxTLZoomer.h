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
#include "ofRange.h"
#include "ofxTLTrack.h"

class ofxTLZoomer : public ofxTLTrack
{
  public:
	ofxTLZoomer();
	virtual ~ofxTLZoomer();
	
	virtual void draw();
		
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	void keyPressed(ofKeyEventArgs& args);
	
	virtual void load();
	virtual void save();

	bool isActive();
	
	//allows for exponential zooming in. Default is 2, no effect is 1
	//not allowed to be less than one
	void setViewExponent(float exponent);
	ofRange getViewRange(); //exponential viewport
	virtual void lostFocus();
	
	//this set is NON exponential, but normalized 0-1
	void setViewRange(ofRange newRange);
    ofRange getSelectedRange(); //non exponential
	
  private:
	void notifyZoomStarted();
	void notifyZoomDragged(ofRange oldZoom);
	void notifyZoomEnded();

	ofRange currentViewRange;

	float minGrabOffset;
	float maxGrabOffset;

	float zoomExponent;
	bool mouseIsDown;
	bool minSelected;
	bool maxSelected;
	bool midSelected;
	
//	bool pointInScreenBounds(ofVec2f screenpoint);
//	float screenXtoRangeX(float x);
//	float rangeXtoScreenX(float x);
};