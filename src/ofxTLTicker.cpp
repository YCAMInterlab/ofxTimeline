/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
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
 * ----------------------
 *
 * ofxTimeline 
 * Lightweight SDK for creating graphic timeline tools in openFrameworks
 */

#include "ofxTLTicker.h"
#include "ofxTimeline.h"

ofxTLTicker::ofxTLTicker() {
	dragging = false;
}

ofxTLTicker::~ofxTLTicker(){
}

void ofxTLTicker::setup(){
	enable();
}

void ofxTLTicker::draw(){
	
	ofPushStyle();
	
	if(timeline->getIsFrameBased()){
		
		int curStartFrame = ofMap(zoomBounds.min, 0, 1.0, 0, timeline->getDurationInFrames());
		int curEndFrame = ofMap(zoomBounds.max, 0, 1.0, 0, timeline->getDurationInFrames());
		int framesInView = curEndFrame-curStartFrame;
	
		float framesPerPixel = framesInView / totalDrawRect.width;
		int frameStepSize = 1;
		
		
		//TODO make adaptive
		//draw ticker marks
		for(int i = curStartFrame; i <= curEndFrame; i++){
			float x = ofMap(i, curStartFrame, curEndFrame, totalDrawRect.x, totalDrawRect.x+totalDrawRect.width, true);
			ofSetColor(200, 180, 40);
			float heightMultiplier = 0.0;
			if(i % 10 == 0){
				ofSetLineWidth(3);
				heightMultiplier = .5;
			}
			else {
				heightMultiplier = .75;
				ofSetLineWidth(1);
			}
			
			ofLine(x, bounds.y+bounds.height*heightMultiplier, x, bounds.y+bounds.height);
		}
		

		//draw current frame
		int currentFrameX = screenXForIndex(timeline->getCurrentFrame());
		string text = ofToString(timeline->getCurrentFrame());
		int textH = 10;
		int textW = (text.size()+1)*7;		
		ofSetColor(timeline->getColors().backgroundColor);
		ofRect(currentFrameX, bounds.y, textW, textH);
		ofSetColor(timeline->getColors().textColor);
		ofDrawBitmapString(text, currentFrameX+5, bounds.y+textH);

		
		if(timeline->getIsPlaying()){
			ofSetColor(timeline->getColors().keyColor);
		}
		else{
			ofSetColor(timeline->getColors().outlineColor);
		}
		
		//draw playhead line
		ofSetLineWidth(1);
		ofLine(currentFrameX, totalDrawRect.y, currentFrameX, totalDrawRect.y+totalDrawRect.height);
		
		
		//highlite current mouse position
		if(hover){
			ofEnableAlphaBlending();
			//draw background rect
			ofSetColor(timeline->getColors().backgroundColor);
			
			int curHoverFrame = indexForScreenX(ofGetMouseX());
			text = ofToString(curHoverFrame);
			textH = 10;
			textW = (text.size()+1)*7;
			ofRect(ofGetMouseX(), bounds.y+textH, textW, textH);
			
			//draw playhead line
			ofSetColor(timeline->getColors().textColor);
			ofDrawBitmapString(text, ofGetMouseX()+5, bounds.y+textH*2);
			
			ofSetColor(timeline->getColors().highlightColor);
			ofSetLineWidth(1);
			ofLine(ofGetMouseX(), totalDrawRect.y, ofGetMouseX(), totalDrawRect.y+totalDrawRect.height);
		}
	}
	//Time based
	else {
		//draw tickers with time
	}

	ofNoFill();
	ofSetColor(200, 180, 40);
	ofRect(bounds);
	
	ofPopStyle();
	
}

void ofxTLTicker::mouseMoved(ofMouseEventArgs& args){
	hover = totalDrawRect.inside(args.x, args.y);
}

void ofxTLTicker::mousePressed(ofMouseEventArgs& args){
	//TODO change playhead position
	dragging = bounds.inside(args.x, args.y);
	if(dragging){
		updateTimelinePosition();
	}
}

void ofxTLTicker::mouseDragged(ofMouseEventArgs& args){
	if(dragging){
		updateTimelinePosition();
	}	
}

void ofxTLTicker::mouseReleased(ofMouseEventArgs& args){
	//TODO change playhead position
	dragging = false;
}

void ofxTLTicker::setTotalDrawRect(ofRectangle drawRect){
	totalDrawRect = drawRect;
}

void ofxTLTicker::updateTimelinePosition(){
	if(timeline->getIsFrameBased()){
		cout << "udpating timeline " << endl;
		timeline->setCurrentFrame(indexForScreenX(ofGetMouseX()));
	}
	else{
		//TODO: timebased scrubbing
//		timeline->setCurrentTime(<#float time#>)
	}
	
}