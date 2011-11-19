/*
 *  ofxTLVideoDepthAlignmentScrubber.h
 *  RGBDPostAlign
 *
 *  Created by James George on 11/16/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofxTLDepthImageSequence.h"
#include "ofxTLVideoPlayer.h"

//TODO add thumbnail
typedef struct{
	int videoFrame;
	int depthFrame;
} VideoDepthPair;

class ofxTLVideoDepthAlignmentScrubber : public ofxTLElement {
  public:
	
	ofxTLVideoDepthAlignmentScrubber();
	~ofxTLVideoDepthAlignmentScrubber();
	
	void setup();
	void draw();
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	void load();
	void save();
	
	void addAlignedPair(int videoFrame, int depthFrame);
	void removeAlignmentPair(int index);
	
	vector<VideoDepthPair> & getPairs();
	
	ofxTLVideoPlayer* videoSequence;
	ofxTLDepthImageSequence* depthSequence;
	
  protected:
	
	bool ready();
	int selectedVideoFrame;
	int selectedDepthFrame;
	int selectedPercent;
	
	int getDepthFrameForVideoFrame(int videoFrame);
	
	vector<VideoDepthPair> alignedFrames;
};