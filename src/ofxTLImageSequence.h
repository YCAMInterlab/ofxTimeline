/*
 *  ofxTLImageSequence.h
 *  RGBDPost
 *
 *  Created by Jim on 10/19/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofxTLImageSequenceFrame.h"

static GLint glTypeForImageType(int imageType){
	if(imageType == OF_IMAGE_GRAYSCALE) return GL_LUMINANCE;
	if(imageType == OF_IMAGE_COLOR) return GL_RGB;
	if(imageType == OF_IMAGE_COLOR_ALPHA) return GL_RGBA;
}

typedef struct
{
	ofTexture* texture;
	ofRectangle bounds;
	int frameIndex;
} PreviewTexture;

class ofxTLImageSequence : public ofxTLElement {
  public:
	ofxTLImageSequence();
	~ofxTLImageSequence();
	
	virtual void setup();
	virtual void draw();
	
	//main function to get values out of the timeline
	virtual bool loadSequence(string directory);
	
	float heightPerFrame;
	
	float getImageWidth();
	float getImageHeight();
	
	float getThumbWidth();
	float getThumbHeight();
	
	vector<PreviewTexture> previewTextures;
	
	ofImage* getImageAtTime(float time);
	ofImage* getImageAtFrame(int frame);

	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void save();
	virtual void load();
	
	virtual void reset();
	virtual void clear();
	
	void purgeFrames();
	void purgeThumbs();
	
  protected:
	void recomputePreview();
	void clearPreviewTextures();
	void clearFrames();
	
	int maxThumbsLoaded;
	int maxFramesLoaded;
    string pathToDirectory;
	
	bool loaded;

	int getIndexAtPercent(float percent);
	
	ofImageType imageType;
	
	float imageWidth, imageHeight;
	float thumbWidth, thumbHeight;


	int framesToShow;
	float widthPerFrame;

	vector<ofxTLImageSequenceFrame*> frames;
	
};