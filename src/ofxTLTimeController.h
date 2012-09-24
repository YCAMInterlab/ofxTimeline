/**
 *
 * Interface for tracks that can control the playhead
 * See ofxTLVideoTrack and ofxTLAudioWaveform for examples
 */

#pragma once
#include "ofMain.h"

class ofxTLTimeController {
  public:
    virtual bool togglePlay() = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual bool isPlaying() = 0;
    
//    virtual void setCurrentTime(float timeInSeconds) = 0;
//    virtual void setCurrentFrame(int frame) = 0;
    
};