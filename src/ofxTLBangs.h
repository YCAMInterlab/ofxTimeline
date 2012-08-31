

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
    //All keyframers could send events...so
    //Potentially this event stuff could be put up in Keyframer, but it's a good distinguisher right now
    virtual void playbackStarted(ofxTLPlaybackEventArgs& args);
	virtual void playbackLooped(ofxTLPlaybackEventArgs& args);
	virtual void playbackEnded(ofxTLPlaybackEventArgs& args);
    
    virtual string getTrackType();
    
 protected:

    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
    bool isPlayingBack;
    void update(ofEventArgs& args);
	virtual void update();
	
    long lastTimelinePoint;

    virtual void bangFired(ofxTLKeyframe* key);
};