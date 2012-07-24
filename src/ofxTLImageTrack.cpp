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
    

    /*
     int frameWidth = int( bounds.height * videoThumbs[0].targetWidth / videoThumbs[0].targetHeight );
     int totalPixels = int( bounds.width / zoomBounds.span() );
     int framesToShow = MAX(totalPixels / frameWidth, 1);
     int frameStep = MAX(videoThumbs.size() / framesToShow, 1); 
     int minPixelIndex = -(zoomBounds.min * totalPixels);
     
     //cout << "bounds are " << bounds.width << " "f  << bounds.height << " frameWidth " << frameWidth << " total pixels " << totalPixels << " frame step " << frameStep << " minpix " << minPixelIndex << endl;
     
     for(int i = 0; i < videoThumbs.size(); i++){
     if(i % frameStep == 0){
     int screenX = screenXForIndex(i);
     videoThumbs[i].displayRect = ofRectangle(screenX, bounds.y, frameWidth, bounds.height);
     videoThumbs[i].visible = videoThumbs[i].displayRect.x+videoThumbs[i].displayRect.width > 0 && videoThumbs[i].displayRect.x < bounds.width;
     }
     else {
     videoThumbs[i].visible = false;
     }
     }
     */
}
