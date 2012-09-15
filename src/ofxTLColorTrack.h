//
//  ofxTLColorTrack.h
//  Duration
//
//  Created by Jim on 8/31/12.
//
//

#pragma once

#include "ofMain.h"
#include "ofxTLBangs.h"

class ofxTLColorSample : public ofxTLKeyframe {
  public:
	ofVec2f samplePoint; //normalized to image space
	ofColor color; //cached sample
};

class ofxTLColorTrack : public ofxTLBangs {
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
	ofColor getColorAtMillis(unsigned long millis);
	ofColor getColorAtPosition(float pos);

	virtual void setDefaultColor(ofColor color);
	virtual ofColor getDefaultColor();
	
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


