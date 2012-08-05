

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframer.h"
#include "ofxTween.h"

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

class ofxTLTweenKeyframe : public ofxTLKeyframe{
  public:
    EasingFunction* easeFunc;
	EasingType* easeType;
};

class ofxTLTweener : public ofxTLKeyframer {
  public:
    ofxTLTweener();
    ~ofxTLTweener();

    virtual void draw();
    virtual void drawModalContent();
    
	//For selecting keyframe type only,
    //the superclass controls keyframe placement
	virtual void mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
    
    virtual void setValueRange(ofRange range, float defaultValue = 0);
	//main function to get values out of the timeline, operates on the given value range
	virtual float getValueAtPercent(float percent);
	virtual float getValueAtTime(long sampleTime);
    
  protected:
    virtual float sampleAtPercent(float percent); //less accurate
    virtual float sampleAtTime(long sampleTime);
    ofRange valueRange;
	float defaultValue;

    virtual ofxTLKeyframe* newKeyframe();
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    
    void selectedKeySecondaryClick(ofMouseEventArgs& args);

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