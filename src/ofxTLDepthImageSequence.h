/*
 *  ofxTLDepthImageSequence.h
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofxTLVideoThumb.h"
#include "ofxKinectPointcloudRecorder.h"

class ofxTLDepthImageSequence : public ofxTLElement {
  public:	
	ofxTLDepthImageSequence();
	~ofxTLDepthImageSequence();

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

	void loadSequence();
	void loadSequence(string sequenceDirectory);
	
	ofImage currentDepthImage;
	unsigned short* currentDepthRaw;
	unsigned short* thumbnailDepthRaw;
	
	int getSelectedFrame();
	void selectFrame(int frame);
	void toggleThumbs();
	
  protected:
	int selectedFrame;
	bool sequenceLoaded;
	
	bool thumbsEnabled;
	
	void calculateFramePositions();
	void generateVideoThumbnails();
	void generateThumbnailForFrame(int index);
	
	
	string sequenceDirectory;
	string thumbDirectory;
	
	ofxKinectPointcloudRecorder decoder;
};
