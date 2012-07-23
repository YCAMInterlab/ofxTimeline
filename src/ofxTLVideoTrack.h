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

//TODO: Playback control system for overall timeline
//TODO: Integrate into ofxTLImageTrack
//TODO: Start and stop points (w handles!) (saving!)
//TODO: Better frame position calculating
//TODO: Don't generate all thumbs up front

class ofxTLVideoTrack : public ofxTLImageTrack, public ofThread {
  public:
	ofxTLVideoTrack();
	~ofxTLVideoTrack();
	
	void setup();
	void draw();
    void loadMovie(string moviePath);
    
    void setPlayer(ofPtr<ofVideoPlayer> newPlayer);
    ofPtr<ofVideoPlayer> getPlayer();
    
	virtual void update(ofEventArgs& args);
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);

	virtual void drawRectChanged();
	
	int getSelectedFrame();
	int getCurrentFrame();

	float getCurrentTime();
	int selectFrame(int frame); //returns the true selected frame in video
	void toggleThumbs();
	
    //Let's the video track have
	void setInFrame(int inFrame);
	void setOutFrame(int outFrame);

  protected:
    
    vector<ofxTLVideoThumb> videoThumbs;

	int selectedFrame;
	int currentLoop;
	bool thumbsEnabled;
	int lastFrame;
		
	int inFrame;
	int outFrame;
	
    float thumbnailUpdatedWidth;
    float thumbnailUpdatedHeight;
    
	void calculateFramePositions();
//	void generateVideoThumbnails();
//	void generateThumbnailForFrame(int index);
//	void purgeOldThumbnails();
    
	ofPtr<ofVideoPlayer> player;
	ofPtr<ofVideoPlayer> backthreadedPlayer;
    
//	string thumbDirectory;	
    bool pauseThumbGeneration;
    void threadedFunction();
    void exit(ofEventArgs& args);
    
};