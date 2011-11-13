/*
 *  ofxTLVideoPlayer.cpp
 *  timelineExampleVideoPlayer
 *
 *  Created by James George on 11/12/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLVideoPlayer.h"

ofxTLVideoPlayer::ofxTLVideoPlayer(){
	player = NULL;	
}

ofxTLVideoPlayer::~ofxTLVideoPlayer(){

}

void ofxTLVideoPlayer::draw(){

}

void ofxTLVideoPlayer::setVideoPlayer(ofVideoPlayer& newPlayer, string thumbDir){
	player = &newPlayer;

	if (!ofDirectory(thumbDir).exists()) {
		ofLogError("ofxTLVideoPlayer -- Directory " + thumbDir + " doesn't exist!");
		return;
	}
	
	thumbDirectory = thumbDir;
	
	for(int i = 0; i < newPlayer.getTotalNumFrames(); i++){
		ofxTLVideoThumb t;
		t.setup(i, thumbDir);
	}
}


void ofxTLVideoPlayer::zoomEnded(ofxTLZoomEventArgs& args){
	
}

