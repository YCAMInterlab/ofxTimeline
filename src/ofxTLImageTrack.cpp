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

#include "ofxTimeline.h"
#include "ofxTLImageTrack.h"

ofxTLImageTrack::ofxTLImageTrack(){
    currentlyZooming = false;
	thumbsEnabled = true;
}


void ofxTLImageTrack::drawRectChanged(){
	calculateFramePositions();
}

void ofxTLImageTrack::zoomStarted(ofxTLZoomEventArgs& args){
    currentlyZooming = true;
	ofxTLTrack::zoomStarted(args);
	calculateFramePositions();
}

void ofxTLImageTrack::zoomDragged(ofxTLZoomEventArgs& args){
	ofxTLTrack::zoomDragged(args);
	calculateFramePositions();
}

void ofxTLImageTrack::zoomEnded(ofxTLZoomEventArgs& args){
    currentlyZooming = false;
	ofxTLTrack::zoomEnded(args);
	calculateFramePositions();
}

void ofxTLImageTrack::calculateFramePositions(){
	
    if(bounds.height < 10 || !isLoaded()){
        return;
    }
    
    //TODO: what if the # of frames in the scene is less than the entire width?
    
    //1) calculate the frame width based on video aspect and height
    float thumbWidth = bounds.height * getContentWidth() / getContentHeight();
    
    //2) find the offset of the first frame against the view 
    float totalTimelineWidthInPixels = bounds.width / zoomBounds.span();
    float pixelsToTheLeft = totalTimelineWidthInPixels * zoomBounds.min;
    
    //3) the offset negative number indicating how far off the left of offscreen the first frame will start
    float pixelOffset = (int(pixelsToTheLeft) / int(thumbWidth) * thumbWidth) - pixelsToTheLeft;
    if(pixelOffset > 0){
        pixelOffset -= thumbWidth;
    }
    //4) find the # of frames that will be in the view
    float numberOfFramesInView = (bounds.width - pixelOffset) / thumbWidth + 1;
    
    //5) create those frames and add them to the vector
    vector<ofxTLVideoThumb> newThumbs;
	for(int i = 0; i < numberOfFramesInView; i++){
        ofxTLVideoThumb thumb;
        thumb.displayRect = ofRectangle(bounds.x + pixelOffset + thumbWidth * i, bounds.y, thumbWidth, bounds.height);
        //TODO: account for offsets at some point
        thumb.timestamp = timeline->screenXToMillis(thumb.displayRect.x);
 		thumb.framenum = timeline->getTimecode().frameForMillis(thumb.timestamp);
//        thumb.framenum = screenXtoNormalizedX(thumb.displayRect.x, zoomBounds) * timeline->getDurationInFrames();
        thumb.loaded = false;
        newThumbs.push_back(thumb);
    }
    
	framePositionsUpdated(newThumbs);
}

string ofxTLImageTrack::getTrackType(){
    return "ImageTrack";
}

