/*
 *  ofxTLVideoPlayer.h
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLVideoThumb.h"
#include "ofxTLElement.h"

class ofxTLVideoPlayer : public ofxTLElement {
  public:
	ofxTLVideoPlayer();
	~ofxTLVideoPlayer();
	
	void setup();
	void draw();
    
	vector<ofxTLVideoThumb> videoThumbs;
    
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

	void setVideoPlayer(ofVideoPlayer& player, string thumbDirectory);
	ofVideoPlayer& getPlayer();
	
	int getSelectedFrame();
	int getCurrentFrame();

	float getCurrentTime();
	int selectFrame(int frame); //returns the true selected frame in video
	void toggleThumbs();
	
	void setInFrame(int inFrame);
	void setOutFrame(int outFrame);
		
  protected:
	int selectedFrame;
	int currentLoop;
	bool thumbsEnabled;
	int lastFrame;
		
	int inFrame;
	int outFrame;
	
    float thumbnailUpdatedWidth;
    float thumbnailUpdatedHeight;
    
	void calculateFramePositions();
	void generateVideoThumbnails();
	void generateThumbnailForFrame(int index);
	//TODO: make this work
	void purgeOldThumbnails();
	
	ofVideoPlayer* player;
	
	string thumbDirectory;	
};