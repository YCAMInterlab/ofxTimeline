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
#include "ofxTLKeyframes.h"
#include "ofxTween.h"

typedef struct {
	int id;
	ofRectangle bounds;
	string name;
	ofPolyline easeInPreview;
	ofPolyline easeOutPreview;
	ofPolyline easeInOutPreview;
	
	ofxEasing* easing;
} EasingFunction;

typedef struct {
	int id;
	ofRectangle bounds;
	string name;
	ofxTween::ofxEasingType type;
} EasingType;

class ofxTLTweenKeyframe : public ofxTLKeyframe{
  public:
    EasingFunction* easeFunc;
	EasingType* easeType;
};

class ofxTLCurves : public ofxTLKeyframes {
  public:
    ofxTLCurves();

//    virtual void draw();
    virtual void drawModalContent();
    
	//For selecting keyframe type only,
    //the superclass controls keyframe placement
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
    virtual string getTrackType();
    
  protected:
	
    virtual ofxTLKeyframe* newKeyframe();
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);	
	virtual float interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long long sampleTime);
	
	//easing dialog stuff
    void initializeEasings();
	ofVec2f easingWindowPosition;
	bool drawingEasingWindow;
	vector<EasingFunction*> easingFunctions;
	vector<EasingType*> easingTypes;
	
	float easingBoxWidth;
	float easingBoxHeight;
	float tweenBoxWidth;
	float tweenBoxHeight;
	
//	float easingWindowSeperatorHeight;

};