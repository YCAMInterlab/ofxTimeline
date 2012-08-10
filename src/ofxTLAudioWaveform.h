/*
 *  ofxTLAudioWaveform.h
 *  audiowaveformTimelineExample
 *
 *  Created by Jim on 12/28/11.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLTrack.h"
#include "ofOpenALSoundPlayer_TimelineAdditions.h"

class ofxTLAudioWaveform : public ofxTLTrack
{
  public:	
	ofxTLAudioWaveform();
	virtual ~ofxTLAudioWaveform();
	
	virtual void enable();
    virtual void disable();
	virtual void draw();
	
	virtual void loadSoundfile(string filepath);
	virtual float getDuration(); //in seconds
	
	virtual void mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	//this will play the timeline along to the song
	virtual void play();
	virtual void stop();
	virtual void togglePlay();
	
	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	virtual void boundsChanged(ofEventArgs& args);
	
	virtual void setSpeed(float speed);
    virtual float getSpeed();
    virtual bool getIsPlaying();
    
	virtual string getTrackType();

  protected:	
    
	bool shouldRecomputePreview;
	vector<ofPolyline> previews;
	void recomputePreview();
	
	float lastPercent;
	virtual void update(ofEventArgs& args);
	ofOpenALSoundPlayer_TimelineAdditions player;
	
	ofRange computedZoomBounds;
};
