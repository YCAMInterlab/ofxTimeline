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
	
	//ui interaction vars -- only set when dragging
	ofVec2f grabOffset; 
} ofxTLKeyframe;

class ofxTLKeyframer : public ofxTLElement
{
  public:	
	ofxTLKeyframer();
	~ofxTLKeyframer();

	virtual void setup();
	virtual void draw();
	virtual void drawModalContent(); //for pop up window-ish stuff

	virtual void setValueRange(ofRange range, float defaultValue = 0);
	//main function to get values out of the timeline, operates on the given value range
	virtual float getValueAtPercent(float percent);
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void keyPressed(ofKeyEventArgs& args);

	virtual void getSnappingPoints(vector<float>& points);
	
	virtual void save();
	virtual void load();
	
	virtual void reset();
	virtual void clear();
	
	//copy paste
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	virtual void unselectAll();
	
  private:
	
	virtual float sampleAt(float percent);

	bool keyframeSelected(ofxTLKeyframe* k);
	
	//vector<ofVec2f> grabOffsets; //keyframe grab offsets for dragging.
	vector<ofxTLKeyframe*> keyframes;
	
	ofRange valueRange;
	float defaultValue;
    
	bool isKeyframeIsInBounds(ofxTLKeyframe* key);
	bool isKeyframeSelected(ofxTLKeyframe* k);

	void nudgeSelectedKeyframes(ofVec2f nudge);
	void deleteSelectedKeyframes();
	
	//ofxTLKeyframe* selectedKeyframe;
	vector<ofxTLKeyframe*> selectedKeyframes;
	ofxTLKeyframe* hoverKeyframe;
	
	int selectedKeyframeIndex;
	bool pointsAreDraggable;
	
	void updateKeyframeSort();
	void updateDragOffsets(ofVec2f screenpoint);

	string getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys);
	void createKeyframesFromXML(ofxXmlSettings xml, vector<ofxTLKeyframe*>& keyContainer);
	
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
	
	bool drawingSelectRect;
	ofVec2f selectRectStartPoint;
	ofRectangle selectRect;
	vector<ofxTLKeyframe*> selectRectSelection;
	
	void initializeEasings();
	ofxTLKeyframe* newKeyframe(ofVec2f point);
};
