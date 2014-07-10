/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLTrack.h"
#include "ofOpenALSoundPlayer_TimelineAdditions.h"

class ofxTLAudioTrack : public ofxTLTrack
{
  public:	
	ofxTLAudioTrack();
	virtual ~ofxTLAudioTrack();
	
	virtual void draw();
	virtual void update();
	
	virtual bool loadSoundfile(string filepath);
	virtual bool isSoundLoaded();
	virtual float getDuration(); //in seconds
	virtual string getSoundfilePath();
	
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	//this will play the timeline along to the audio
    virtual bool togglePlay();
    virtual void play();
    virtual void stop();
    virtual bool getIsPlaying();

    virtual void setFFTDampening(float dampening);
    virtual float getFFTDampening();
    
    virtual void setUseFFTEnvelope(bool useEnveolope);
    virtual bool getUseFFTEnvelope();


	virtual void zoomStarted(ofxTLZoomEventArgs& args);
	virtual void zoomDragged(ofxTLZoomEventArgs& args);
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
    virtual void playbackStarted(ofxTLPlaybackEventArgs& args);
	virtual void playbackLooped(ofxTLPlaybackEventArgs& args);
	virtual void playbackEnded(ofxTLPlaybackEventArgs& args);

	virtual void boundsChanged(ofEventArgs& args);
	
	virtual void setSpeed(float speed);
    virtual float getSpeed();
	virtual void setVolume(float volume);
	virtual void setPan(float pan);
    
	virtual string getTrackType();

    //FFT for audio reactive
    void setFFTLogAverages(int minBandwidth = 88, int bandsPerOctave = 20);
    int getLogAverageMinBandwidth();
    int getLogAverageBandsPerOctave();
    
    int getFFTSize();
	vector<float>& getFFT();
    int getBufferSize();
    vector<float> &getCurrentBuffer(int _size = 512);
    vector<float> &getBufferForFrame(int _frame, int _size = 512);

  protected:
	
	float positionForSecond(float second);
    bool soundLoaded;
	bool shouldRecomputePreview;
	vector<ofPolyline> previews;
	void recomputePreview();
	string soundFilePath;
	float lastFFTPosition;
    float lastBufferPosition;
	int defaultSpectrumBandwidth;

    vector<float> dampened;
    vector<float> buffered;
    
	float lastPercent;
    
//	virtual void update(ofEventArgs& args);
	ofOpenALSoundPlayer_TimelineAdditions player;
	ofRange computedZoomBounds;
	float maxBinReceived;
    float dampening;

    void generateEnvelope(int size);
    int averageSize;
    bool useEnvelope;
    vector<float> envelope;
};
