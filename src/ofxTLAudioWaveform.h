/*
 *  ofxTLAudioWaveform.h
 *  audiowaveformTimelineExample
 *
 *  Created by Jim on 12/28/11.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofOpenALSoundPlayer.h"

class ofxTLAudioWaveform : public ofxTLElement
{
  public:	
	ofxTLAudioWaveform();
	~ofxTLAudioWaveform();
	
	virtual void setup();
	virtual void draw();
	
	virtual void loadSoundfile(string filepath);
	virtual float getDuration(); //in seconds
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	//this will play the timeline along to the song
	virtual void play();
	virtual void stop();
	virtual void togglePlay();
	
	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	virtual void boundsChanged(ofEventArgs& args);
	
  protected:	
	bool zoomChanging;
	bool shouldRecomputePreview;
	vector<ofPolyline> previews;
	void recomputePreview();
	
	float lastPercent;
	virtual void update(ofEventArgs& args);
	ofOpenALSoundPlayer player;
	
	ofRange computedZoomBounds;
};