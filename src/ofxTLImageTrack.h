
#pragma once

#include "ofMain.h"

#include "ofxTLTrack.h"
#include "ofxTLVideoThumb.h"

class ofxTLImageTrack : public ofxTLTrack {
  public:
	ofxTLImageTrack();
    virtual ~ofxTLImageTrack();
    
    virtual bool isLoaded() = 0;
    
    virtual void zoomStarted(ofxTLZoomEventArgs& args);
    virtual void zoomDragged(ofxTLZoomEventArgs& args);
    virtual void zoomEnded(ofxTLZoomEventArgs& args);

    virtual void drawRectChanged();
    
    virtual string getTrackType();
    
  protected:
    
    bool currentlyZooming;
    vector<ofxTLVideoThumb> videoThumbs;
	virtual void calculateFramePositions();
    
    //width and height of image elements
    virtual float getContentWidth() = 0;
    virtual float getContentHeight() = 0;
	virtual void framePositionsUpdated(vector<ofxTLVideoThumb>& newThumbs) = 0;
};