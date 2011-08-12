/*
 *  ofxTLEvents.h
 *  timelineExample
 *
 *  Created by James George on 8/9/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofRange.h"

class ofxTLPlaybackEventArgs : public ofEventArgs {
  public: 	
	float currentTime;
	int currentFrame;
	float durationInSeconds;
	int durationInFrames;
};

class ofxTLZoomEventArgs : public ofEventArgs {
  public:
	ofRange currentZoom;
	ofRange oldZoom;
};


class ofxTLCoreEvents {
  public:
	ofEvent<ofxTLPlaybackEventArgs> playbackStarted;
	ofEvent<ofxTLPlaybackEventArgs> playbackEnded;
	ofEvent<ofxTLPlaybackEventArgs> playbackLooped;
	
	ofEvent<ofxTLZoomEventArgs> zoomStarted;
	ofEvent<ofxTLZoomEventArgs> zoomDragged;
	ofEvent<ofxTLZoomEventArgs> zoomEnded;
	
	ofEvent<ofEventArgs> viewNeedsResize;
};

extern ofxTLCoreEvents ofxTLEvents;

template<class ListenerClass>
void ofxTLRegisterPlaybackEvents(ListenerClass * listener){
    ofAddListener(ofxTLEvents.playbackStarted, listener, &ListenerClass::playbackBegan);
    ofAddListener(ofxTLEvents.playbackEnded, listener, &ListenerClass::playbackEnded);
    ofAddListener(ofxTLEvents.playbackLooped, listener, &ListenerClass::playbackLooped);
}

template<class ListenerClass>
void ofxTLRemovePlaybackEvents(ListenerClass * listener){
    ofRemoveListener(ofxTLEvents.playbackStarted, listener, &ListenerClass::playbackBegan);
    ofRemoveListener(ofxTLEvents.playbackEnded, listener, &ListenerClass::playbackEnded);
    ofRemoveListener(ofxTLEvents.playbackLooped, listener, &ListenerClass::playbackLooped);
}

template<class ListenerClass>
void ofxTLRegisterZoomEvents(ListenerClass * listener){
    ofAddListener(ofxTLEvents.zoomStarted, listener, &ListenerClass::zoomStarted);
    ofAddListener(ofxTLEvents.zoomDragged, listener, &ListenerClass::zoomDragged);
    ofAddListener(ofxTLEvents.zoomEnded, listener, &ListenerClass::zoomEnded);
}

template<class ListenerClass>
void ofxTLRemoveZoomEvents(ListenerClass * listener){
    ofRemoveListener(ofxTLEvents.zoomStarted, listener, &ListenerClass::zoomStarted);
    ofRemoveListener(ofxTLEvents.zoomDragged, listener, &ListenerClass::zoomDragged);
    ofRemoveListener(ofxTLEvents.zoomEnded, listener, &ListenerClass::zoomEnded);
}
