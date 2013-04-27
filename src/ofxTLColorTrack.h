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
#include "ofxTLKeyframes.h"

class ofxTLColorSample : public ofxTLKeyframe {
  public:
	ofVec2f samplePoint; //normalized to image space
	ofColor color; //cached sample
};

class ofxTLColorTrack : public ofxTLKeyframes {
  public:
    ofxTLColorTrack();
	
	virtual void draw();
    virtual void drawModalContent();
    
	//For selecting keyframe type only,
    //the superclass controls keyframe placement
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	virtual void keyPressed(ofKeyEventArgs& args);

    virtual string getTrackType();
	
	virtual void loadColorPalette(ofBaseHasPixels& image);
	virtual bool loadColorPalette(string imagePath);
	virtual string getPalettePath(); //only valid when it's been loaded from an image path
	
    ofColor getColor();
	ofColor getColorAtSecond(float second);
	ofColor getColorAtMillis(unsigned long long millis);
	ofColor getColorAtPosition(float pos);

	virtual void setDefaultColor(ofColor color);
	virtual ofColor getDefaultColor();
	virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);
	
  protected:
	ofImage colorPallete;
	ofImage previewPalette;
	string palettePath;
	
	virtual void updatePreviewPalette();
	virtual ofxTLKeyframe* newKeyframe();
    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
	
	ofColor colorAtClickTime;
	ofVec2f samplePositionAtClickTime;
	bool clickedInColorRect;
	bool drawingColorWindow;
	bool setNextAndPreviousOnUpdate;
	ofRectangle colorWindow;
	ofRectangle previousColorRect;
	ofRectangle newColorRect;
	
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);
	
	void refreshAllSamples();
	//set when selecting
	void setNextAndPreviousSamples();
	ofxTLColorSample* previousSample;
	ofxTLColorSample* nextSample;
	void refreshSample(ofxTLColorSample* sample);
	ofColor samplePaletteAtPosition(ofVec2f position);
	
	ofColor defaultColor;
	
};


