
#pragma once

#include "ofMain.h"

#include "ofxTLTrack.h"
#include "ofxTLVideoThumb.h"

class ofxTLImageTrack : public ofxTLTrack {
  public:
	ofxTLImageTrack();
    ~ofxTLImageTrack();
    
    void setup();

  protected:
    
    vector<ofxTLVideoThumb> videoThumbs;
	void calculateFramePositions();
    
    virtual bool canCalculateThumbs() = 0;
    
    //width and height of image elements
    virtual float getContentWidth() = 0;
    virtual float getContentHeight() = 0;
	virtual void framePositionsUpdated(vector<ofxTLVideoThumb>& newThumbs) = 0;
};