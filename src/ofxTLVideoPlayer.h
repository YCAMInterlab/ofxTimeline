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

	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);

	virtual void drawRectChanged();

	void setVideoPlayer(ofVideoPlayer& player, string thumbDirectory);
	int getSelectedFrame();
	void selectFrame(int frame);
	void toggleThumbs();
	
  protected:
	int selectedFrame;
	bool thumbsEnabled;
	
	void calculateFramePositions();
	void generateVideoThumbnails();
	void generateThumbnailForFrame(int index);
	//TODO: make this work
	void purgeOldThumbnails();
	
	ofVideoPlayer* player;
	
	string thumbDirectory;	
};