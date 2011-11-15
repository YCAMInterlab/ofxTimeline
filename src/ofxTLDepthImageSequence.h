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
#include "ofxTLDepthImageSequenceFrame.h"

class ofxTLDepthImageSequence : public ofxTLElement {
  public:	
	ofxTLVideoPlayer();
	~ofxTLVideoPlayer();

	void setup();
	void draw();

	vector<ofxTLDepthImageSequenceFrame> videoThumbs;

	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);

	virtual void drawRectChanged();

	void loadDepthSequence();
	void loadDepthSequence(string sequenceDirectory);
	
  protected:
	string sequenceDirectory;
};
