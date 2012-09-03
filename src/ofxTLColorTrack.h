//
//  ofxTLColorTrack.h
//  Duration
//
//  Created by Jim on 8/31/12.
//
//

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
    virtual string getTrackType();
	
	virtual void loadColorPalette(ofBaseHasPixels& image);
	virtual void loadColorPalette(string imagePath);
	virtual string getPalettePath(); //only valid when it's been loaded from an image path
	
    ofColor getColor();
	ofColor getColorAtSecond(float second);
	ofColor getColorAtMillis(unsigned long millis);
	ofColor getColorAtPosition(float pos);

	virtual void setDefaultColor(ofColor color);
	virtual ofColor getDefaultColor();
	
//	virtual void load();
//	virtual void save();
	
  protected:
	
	ofImage colorPallete;
	ofImage previewPalette;
	string palettePath;
	
	bool shouldRecalculatePreview;
	
	virtual void updatePreviewPalette();
	virtual ofxTLKeyframe* newKeyframe();
	
    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
	bool clickedInColorRect;
	bool drawingColorWindow;
	ofRectangle colorWindow;
	
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);
	
	void refreshSample(ofxTLColorSample* sample);
	ofColor samplePaletteAtPosition(ofVec2f position);
	
	ofColor defaultColor;
	
};


