/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxTimeline 
 * Lightweight SDK for creating graphic timeline tools in openFrameworks
 */

#pragma once

#include "ofMain.h"
#include "ofRange.h"

class ofxTLPlaybackEventArgs : public ofEventArgs {
  public: 	
	float currentPercent; //always valid
	bool frameBased; //use this to decide whether to look at frames or seconds
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

class ofxTLPageEventArgs : public ofEventArgs {
  public:
	string currentPageName;
	string oldPageName;
};

class ofxTLTriggerEventArgs : public ofEventArgs {
  public:
	string triggerGroupName;
	string triggerName;
};

class ofxTLSwitchEventArgs : public ofEventArgs {
  public:
	string switchName;
	bool on;
};

class ofxTLCoreEvents {
  public:
	ofEvent<ofxTLPlaybackEventArgs> playbackStarted;
	ofEvent<ofxTLPlaybackEventArgs> playbackEnded;
	ofEvent<ofxTLPlaybackEventArgs> playbackLooped;
	
	ofEvent<ofxTLZoomEventArgs> zoomStarted;
	ofEvent<ofxTLZoomEventArgs> zoomDragged;
	ofEvent<ofxTLZoomEventArgs> zoomEnded;

	ofEvent<ofxTLTriggerEventArgs> trigger;

	ofEvent<ofxTLSwitchEventArgs> switched;
	
	ofEvent<ofxTLPageEventArgs> pageChanged;
		
	ofEvent<ofEventArgs> viewWasResized;
};

extern ofxTLCoreEvents ofxTLEvents;

template<class ListenerClass>
void ofxTLRegisterPlaybackEvents(ListenerClass * listener){
    ofAddListener(ofxTLEvents.playbackStarted, listener, &ListenerClass::playbackStarted);
    ofAddListener(ofxTLEvents.playbackEnded, listener, &ListenerClass::playbackEnded);
    ofAddListener(ofxTLEvents.playbackLooped, listener, &ListenerClass::playbackLooped);
}

template<class ListenerClass>
void ofxTLRemovePlaybackEvents(ListenerClass * listener){
    ofRemoveListener(ofxTLEvents.playbackStarted, listener, &ListenerClass::playbackStarted);
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
