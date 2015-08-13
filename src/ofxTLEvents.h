/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
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
 */

#pragma once

#include "ofMain.h"
#include "ofRange.h"

class ofxTimeline; //forward declare for sender pointer
class ofxTLTrack;
class ofxTLPlaybackEventArgs : public ofEventArgs {
  public: 	
    ofxTimeline* sender;
	float currentPercent;
	float currentTime;
	int currentFrame;
	float durationInSeconds;
	int durationInFrames;
};

class ofxTLZoomEventArgs : public ofEventArgs {
  public:
    ofxTimeline* sender;
	ofRange currentZoom;
	ofRange oldZoom;
};

class ofxTLInOutEventArgs : public ofEventArgs {
public:
    ofxTimeline* sender;
    ofRange inoutRange;
};

class ofxTLPageEventArgs : public ofEventArgs {
  public:
    ofxTimeline* sender;
	string currentPageName;
	string oldPageName;
};

class ofxTLTrackEventArgs : public ofEventArgs {
public:
    ofxTimeline* sender;
    ofxTLTrack* track;
	string name;
	string displayName;
};

class ofxTLBangEventArgs : public ofEventArgs {
  public:
    ofxTimeline* sender;
	ofxTLTrack* track;
	float currentPercent;
	float currentTime;
	int currentFrame;
    long currentMillis;
	string flag;
};

class ofxTLSwitchEventArgs : public ofEventArgs {
  public:
    ofxTimeline* sender;
    ofxTLTrack* track;
	string switchName;
	bool on;
};

class ofxTLEvents {
  public:
	ofEvent<ofxTLPlaybackEventArgs> playbackStarted;
	ofEvent<ofxTLPlaybackEventArgs> playbackEnded;
	ofEvent<ofxTLPlaybackEventArgs> playbackLooped;
	ofEvent<ofxTLPlaybackEventArgs> playheadScrubbed;

    ofEvent<ofxTLInOutEventArgs> inOutChanged;

	ofEvent<ofxTLZoomEventArgs> zoomStarted;
	ofEvent<ofxTLZoomEventArgs> zoomDragged;
	ofEvent<ofxTLZoomEventArgs> zoomEnded;

	ofEvent<ofxTLBangEventArgs> bangFired;

	ofEvent<ofxTLSwitchEventArgs> switched;
	
	ofEvent<ofxTLPageEventArgs> pageChanged;
    
    ofEvent<ofxTLTrackEventArgs> trackGainedFocus;
    ofEvent<ofxTLTrackEventArgs> trackLostFocus;
		
	ofEvent<ofEventArgs> viewWasResized;


    template<class ListenerClass>
    void registerPlaybackEvents(ListenerClass * listener){
        ofAddListener(playbackStarted, listener, &ListenerClass::playbackStarted);
        ofAddListener(playbackEnded, listener, &ListenerClass::playbackEnded);
        ofAddListener(playbackLooped, listener, &ListenerClass::playbackLooped);
    }

    template<class ListenerClass>
    void removePlaybackEvents(ListenerClass * listener){
        ofRemoveListener(playbackStarted, listener, &ListenerClass::playbackStarted);
        ofRemoveListener(playbackEnded, listener, &ListenerClass::playbackEnded);
        ofRemoveListener(playbackLooped, listener, &ListenerClass::playbackLooped);
    }

    template<class ListenerClass>
    void registerZoomEvents(ListenerClass * listener){
        ofAddListener(zoomStarted, listener, &ListenerClass::zoomStarted);
        ofAddListener(zoomDragged, listener, &ListenerClass::zoomDragged);
        ofAddListener(zoomEnded, listener, &ListenerClass::zoomEnded);
	}

    template<class ListenerClass>
    void removeZoomEvents(ListenerClass * listener){
        ofRemoveListener(zoomStarted, listener, &ListenerClass::zoomStarted);
        ofRemoveListener(zoomDragged, listener, &ListenerClass::zoomDragged);
        ofRemoveListener(zoomEnded, listener, &ListenerClass::zoomEnded);
    }
};