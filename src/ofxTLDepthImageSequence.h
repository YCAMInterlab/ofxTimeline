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
#include "ofxKinectPointcloudDecoder.h"

class ofxTLDepthImageSequence : public ofxTLElement {
  public:	
	ofxTLDepthImageSequence();
	~ofxTLDepthImageSequence();

	void setup();
	void draw();

	vector<ofxTLVideoThumb> videoThumbs;

	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);

	virtual void drawRectChanged();

	void loadSequence();
	void loadSequence(string sequenceDirectory);
	
	
  protected:
	bool sequenceLoaded;
	
	void calculateFramePositions();
	void generateVideoThumbnails();
	void generateThumbnailForFrame(int index);
	
	int indexForMousePoint(int mouseX);
	string sequenceDirectory;
	string thumbDirectory;
	
	
	ofxKinectPointcloudDecoder decoder;
};
