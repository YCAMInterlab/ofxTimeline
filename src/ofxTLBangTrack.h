

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframer.h"

class ofxTLBangTrack : public ofxTLKeyframer {
  public:
    ofxTLBangTrack();
	~ofxTLBangTrack();
	
    virtual void enable();
    virtual void disable();
    
    virtual void draw();

    //All keyframers could send events...so
    //Potentially this event stuff could be put up in Keyframer, but it's a good distinguisher right now
    void playbackStarted(ofxTLPlaybackEventArgs& args);
	void playbackLooped(ofxTLPlaybackEventArgs& args);
	void playbackEnded(ofxTLPlaybackEventArgs& args);

    
 protected:

    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
    void update(ofEventArgs& args);
    float lastTimelinePoint;

    virtual void bangFired(ofxTLKeyframe* key);
};