#include "ofxTimeline.h"
#include "ofxTLImageTrack.h"

ofxTLImageTrack::ofxTLImageTrack(){
    currentlyZooming = false;
}

ofxTLImageTrack::~ofxTLImageTrack(){

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
        thumb.displayRect = ofRectangle(pixelOffset + thumbWidth * i, bounds.y, thumbWidth, bounds.height);
        //TODO: account for offsets at some point
        thumb.framenum = screenXtoNormalizedX(thumb.displayRect.x, zoomBounds) * timeline->getDurationInFrames();
        thumb.timestamp = timeline->getTimecode().millisForFrame(thumb.framenum);
        thumb.loaded = false;
        newThumbs.push_back(thumb);
    }
    
	framePositionsUpdated(newThumbs);
}

string ofxTLImageTrack::getTrackType(){
    return "ImageTrack";
}

