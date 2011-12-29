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
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	

  protected:
	ofOpenALSoundPlayer player;
};