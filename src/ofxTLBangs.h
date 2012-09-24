

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframes.h"

class ofxTLBangs : public ofxTLKeyframes {
  public:
    ofxTLBangs();
	virtual ~ofxTLBangs();
	
    virtual void enable();
    virtual void disable();
    
    virtual void draw();

    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);
	
    virtual void playbackStarted(ofxTLPlaybackEventArgs& args);
	virtual void playbackLooped(ofxTLPlaybackEventArgs& args);
	virtual void playbackEnded(ofxTLPlaybackEventArgs& args);
    
    virtual string getTrackType();
    
 protected:

    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
    bool isPlayingBack;
	virtual void update();
	
    long lastTimelinePoint;
	float lastBangTime; //just for display
	
    virtual void bangFired(ofxTLKeyframe* key);
};