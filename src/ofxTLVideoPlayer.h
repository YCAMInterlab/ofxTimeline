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
#include "ofxTLElement.h"
#include "ofxTLVideoThumb.h"

class ofxTLVideoPlayer : public ofxTLElement {
  public:
	ofxTLVideoPlayer();
	~ofxTLVideoPlayer();
	
	void setup();
	void draw();
	vector<ofxTLVideoThumb> videoThumbs;
	
	void setVideoPlayer(ofVideoPlayer& player, string thumbDirectory);
	
  protected:
	ofVideoPlayer* player;
	string thumbDirectory;
};