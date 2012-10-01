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

class ofxTLImageSequence : public ofxTLTrack {
  public:
	ofxTLImageSequence();
	virtual ~ofxTLImageSequence();
	
	virtual void setup();
	virtual void draw();
	
	//main function to get values out of the timeline
	virtual bool loadSequence(string directory);
	
	float getImageWidth();
	float getImageHeight();
	
	float getThumbWidth();
	float getThumbHeight();
	
	vector<PreviewTexture> previewTextures;
	
	ofImage* getImageAtTime(float time);
	ofImage* getImageAtFrame(int frame);

	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void save();
	virtual void load();
	
	virtual void reset();
	virtual void clear();
	
	virtual void drawRectChanged();
	virtual void setZoomBounds(ofRange zoomBoundsPercent);
		
	void purgeFrames();
	
    virtual string getTrackType();
    
  protected:
    
    int historySize; //how many images to keep in the history queue
    
	void recomputePreview();
	void clearPreviewTextures();
	void clearFrames();
	
	int maxThumbsLoaded;
	int maxFramesLoaded;
    string pathToDirectory;
	
	bool loaded;

	int getIndexAtPercent(float percent);
	float getPercentAtIndex(int index);
	
	ofImageType imageType;
	
	float imageWidth, imageHeight;
	float thumbWidth, thumbHeight;

	vector<ofxTLImageSequenceFrame*> frames;
	
};