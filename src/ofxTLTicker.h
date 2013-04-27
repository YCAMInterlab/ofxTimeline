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

typedef struct{
	float screenX;
    unsigned long long millis;
	int weight;
} ofxTLBPMPoint;

class ofxTLTicker : public ofxTLTrack
{
  public:
	ofxTLTicker();
	~ofxTLTicker();
	
	virtual void draw();
	
	//set the draw rect for the whole keyframer interface
	virtual void setTotalDrawRect(ofRectangle drawRect);
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);

	virtual float getBPM();
	virtual void setBPM(float bpm);
    
	virtual void getSnappingPoints(set<unsigned long long>& points);
	virtual bool getDrawBPMGrid();
	virtual void setDrawBPMGrid(bool drawGrid);
	virtual void setHoverTime(unsigned long long millis);
	
  protected:
	void updateTimelinePosition();
	void updateBPMPoints();

	ofRectangle totalDrawRect;
	vector<ofxTLBPMPoint> bpmScreenPoints;
    unsigned long long hoverTime;
	bool hasBPM;
	float bpm;
	bool drawBPMGrid;
	bool dragging;
	bool playOnMouseReleased;
    
	ofPath tickerMarks;
	void refreshTickMarks();
};
