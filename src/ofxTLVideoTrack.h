/*
 *  ofxTLVideoTrack.h
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLVideoThumb.h"
#include "ofxTLImageTrack.h"
#include "ofxTLTimeController.h"

//TODO: Playback control system for overall timeline
//TODO: Start and stop points (w handles!) (saving!)

class ofxTLVideoTrack : public ofxTLImageTrack, public ofThread, public ofxTLTimeController {
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
    
	virtual void update(ofEventArgs& args);
	virtual void mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	
	virtual void keyPressed(ofKeyEventArgs& args);
		
	int getSelectedFrame();
	int getCurrentFrame();

    bool isLoaded();
    
	float getCurrentTime();
	int selectFrame(int frame); //returns the true selected frame in video
	void toggleThumbs();
	
    //Let's the video track have
	void setInFrame(int inFrame);
	void setOutFrame(int outFrame);

    virtual bool togglePlay();
    virtual void play();
    virtual void stop();
    virtual bool isPlaying();
    	
    virtual string getTrackType();
    
  protected:
    
	int selectedFrame;
	int currentLoop;
	bool thumbsEnabled;
		
	int inFrame;
	int outFrame;
	
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