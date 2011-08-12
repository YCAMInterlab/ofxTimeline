/*
 *  THISKeyframeEditor.h
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
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
	
	//main function to get values out of the timeline
	float sampleTimelineAt(float percent);
	
	void draw();
	
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
		
	void keyPressed(ofKeyEventArgs& args);

	void save();
	void load();
	
	void reset();
	void clear();
	
  private:
	ofxTLKeyframe* firstkey;
	ofxTLKeyframe* lastkey;
	ofVec2f grabOffset;
	vector<ofxTLKeyframe*> keyframes;

	
	bool keyframeIsInBounds(ofxTLKeyframe* key);
	
	ofxTLKeyframe* selectedKeyframe;
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