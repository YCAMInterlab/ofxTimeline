

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframes.h"
#include "ofxTween.h"

typedef struct {
	int id;
	ofRectangle bounds;
	string name;
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
	virtual void mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	
    virtual string getTrackType();
    
  protected:
	
    virtual ofxTLKeyframe* newKeyframe();
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    
    void selectedKeySecondaryClick(ofMouseEventArgs& args);
	
	float interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long sampleTime);
	
	
	//easing dialog stuff
    void initializeEasings();
	ofVec2f easingWindowPosition;
	bool drawingEasingWindow;
	vector<EasingFunction*> easingFunctions;
	vector<EasingType*> easingTypes;
	float easingBoxWidth;
	float easingBoxHeight;
	float easingWindowSeperatorHeight;

};