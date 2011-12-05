/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
 * implementaiton by James George (@obviousjim) and Tim Gfrerer (@tgfrerer) for the 
 * Voyagers gallery National Maritime Museum 
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
#include "ofxTween.h"
#include "ofxTLElement.h"

typedef struct
{
	int id;
	ofRectangle bounds;
	string name;
	ofxEasing* easing;
} EasingFunction;

typedef struct
{
	int id;
	ofRectangle bounds;
	string name;
	ofxTween::ofxEasingType type;
} EasingType;

typedef struct
{
	EasingFunction* easeFunc;
	EasingType* easeType;
	ofVec2f position; // x is value, y is time, all 0 - 1.0
} ofxTLKeyframe;

class ofxTLKeyframer : public ofxTLElement
{
  public:	
	ofxTLKeyframer();
	~ofxTLKeyframer();

	virtual void setup();
	virtual void draw();

	virtual void setValueRange(ofRange range);
	//main function to get values out of the timeline, operates on the given value range
	virtual float getValueAtPercent(float percent);
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
		
	virtual void keyPressed(ofKeyEventArgs& args);

	virtual void save();
	virtual void load();
	
	virtual void reset();
	virtual void clear();
	
  private:
	
	virtual float sampleAt(float percent);

	ofVec2f grabOffset;
	vector<ofxTLKeyframe*> keyframes;

	ofRange valueRange;
	
	bool keyframeIsInBounds(ofxTLKeyframe* key);
	
	ofxTLKeyframe* selectedKeyframe;
	ofxTLKeyframe* hoverKeyframe;
	
	int selectedKeyframeIndex;
	ofVec2f keyframeGrabOffset;
	float minBound; //TODO: replace with range
	float maxBound;
	
	void updateKeyframeSort();
	ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
	bool screenpointIsInBounds(ofVec2f screenpoint);
	ofVec2f coordForKeyframePoint(ofVec2f keyframePoint);
	ofVec2f keyframePointForCoord(ofVec2f coord);
		
	//easing dialog stuff
	ofVec2f easingWindowPosition;
	bool drawingEasingWindow;
	vector<EasingFunction*> easingFunctions;
	vector<EasingType*> easingTypes;
	float easingBoxWidth;
	float easingBoxHeight;
	float easingWindowSeperatorHeight;
	
	void initializeEasings();
	ofxTLKeyframe* newKeyframe(ofVec2f point);
};
