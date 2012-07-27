

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframer.h"

class ofxTLBangs : public ofxTLKeyframer {
  public:
    ofxTLBangs();
	~ofxTLBangs();
	
    virtual void enable();
    virtual void disable();
    
    virtual void draw();

    virtual void regionSelected(ofRange timeRange, ofRange valueRange);
    //All keyframers could send events...so
    //Potentially this event stuff could be put up in Keyframer, but it's a good distinguisher right now
    virtual void playbackStarted(ofxTLPlaybackEventArgs& args);
	virtual void playbackLooped(ofxTLPlaybackEventArgs& args);
	virtual void playbackEnded(ofxTLPlaybackEventArgs& args);
    
 protected:

    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
    void update(ofEventArgs& args);
    float lastTimelinePoint;

    virtual void bangFired(ofxTLKeyframe* key);
};