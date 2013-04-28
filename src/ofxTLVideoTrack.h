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
#include "ofxTLVideoThumb.h"
#include "ofxTLImageTrack.h"

//TODO: Start and stop points (w handles!) (saving!)

class ofxTLVideoTrack : public ofxTLImageTrack, public ofThread {
  public:
	ofxTLVideoTrack();
	virtual ~ofxTLVideoTrack();
	
	virtual void setup();
	virtual void draw();
    virtual void enable();
    virtual void disable();
    
    bool load(string moviePath);

    void setPlayer(ofVideoPlayer& newPlayer);
    void setPlayer(ofPtr<ofVideoPlayer> newPlayer);
    ofPtr<ofVideoPlayer> getPlayer();
    
	virtual void update();
//	virtual void update(ofEventArgs& args);
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	
	virtual void keyPressed(ofKeyEventArgs& args);
			
    virtual void playbackStarted(ofxTLPlaybackEventArgs& args);
	virtual void playbackLooped(ofxTLPlaybackEventArgs& args);
	virtual void playbackEnded(ofxTLPlaybackEventArgs& args);
	
	int getSelectedFrame();
	int getCurrentFrame();
	float getCurrentTime();

    void setDrawVideoPreview(bool drawPreview);
    bool getDrawVideoPreview();

    bool isLoaded();
	int selectFrame(int frame); //returns the true selected frame in video
	void toggleThumbs();
	
    //Let's the video track have
	void setInFrame(int inFrame);
	void setOutFrame(int outFrame);

    virtual bool togglePlay();
    virtual void play();
    virtual void stop();
    virtual bool getIsPlaying();
    	
    virtual string getTrackType();
    
  protected:
    float positionForSecond(float second);
	
	int selectedFrame;
	int currentLoop;
//	bool thumbsEnabled;
		
	int inFrame;
	int outFrame;
    
	bool drawVideoPreview;
    bool canCalculateThumbs();
    bool currentlyPlaying;
    
    //width and height of image elements
    float getContentWidth();
    float getContentHeight();

    void framePositionsUpdated(vector<ofxTLVideoThumb>& newThumbs);
	ofPtr<ofVideoPlayer> player;
	ofPtr<ofVideoPlayer> backthreadedPlayer; //this generates thumbnails - a memory compromise to have 2 videos but but speeds things up big time
	ofMutex backLock; // to protect backThumbs
    vector<ofxTLVideoThumb> backThumbs; //used to generate thumbs on the back thread, then copies them onto the main thread
    
	void playheadScrubbed(ofxTLPlaybackEventArgs& args);
        
    void threadedFunction();
    void exit(ofEventArgs& args);
    
};