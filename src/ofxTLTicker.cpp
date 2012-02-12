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
	hasBPM = false;
	drawBPMGrid = false;
}

void ofxTLTicker::draw(){
	
	ofPushStyle();
	
	int textH, textW;
	string text;
	if(timeline->getIsFrameBased()){
		
		int curStartFrame = ofMap(zoomBounds.min, 0, 1.0, 0, timeline->getDurationInFrames());
		int curEndFrame = ofMap(zoomBounds.max, 0, 1.0, 0, timeline->getDurationInFrames());
		int framesInView = curEndFrame-curStartFrame;
	
		float framesPerPixel = framesInView / bounds.width;
		int frameStepSize = 1;
		
		//TODO make adaptive if we are way zoomed in don't draw so many
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
				ofSetLineWidth(1);
				heightMultiplier = .75;
			}
			
			ofLine(x, bounds.y+bounds.height*heightMultiplier, x, bounds.y+bounds.height);
		}
		
		
	}
	//Time based
	else {
		//draw tickers with time
		float startTime = zoomBounds.min * timeline->getDurationInSeconds();
		float endTime = zoomBounds.max * timeline->getDurationInSeconds();
		float durationInview = endTime-startTime;
		float secondsPerPixel = durationInview / bounds.width;
		
		//draw ticker marks
		ofSetLineWidth(1);
		ofSetColor(200, 180, 40);
		float heightMultiplier = .75;
		for(float i = startTime; i <= endTime; i += secondsPerPixel*5){
			//float x = ofMap(i, curStartFrame, curEndFrame, totalDrawRect.x, totalDrawRect.x+totalDrawRect.width, true);
			float x = screenXForTime(i);
			ofLine(x, bounds.y+bounds.height*heightMultiplier, x, bounds.y+bounds.height);
		}
		
		//draw regular increments
		int bigTickStep;
		if(durationInview < 1){ //draw big tick every 100 millis
			bigTickStep = .1;
		}
		else if(durationInview < 60){ // draw big tick every second
			bigTickStep = 1;
		}
		else {
			bigTickStep = 60;
		}
		ofSetLineWidth(3);
		heightMultiplier = .5;		
		for(float i = startTime-fmod(startTime, bigTickStep); i <= endTime; i+=bigTickStep){
			float x = screenXForTime(i);
			ofLine(x, bounds.y+bounds.height*heightMultiplier, x, bounds.y+bounds.height);
		}
		
		if(drawBPMGrid){
			updateBPMPoints();
			ofPushStyle();
			ofSetColor(255, 255, 255, 50);
			for(int i = 0; i < bpmScreenPoints.size(); i++){
				ofSetLineWidth(bpmScreenPoints[i].weight);
				ofLine(bpmScreenPoints[i].screenX, totalDrawRect.y, bpmScreenPoints[i].screenX, totalDrawRect.y+totalDrawRect.height-20);
			}
			ofPopStyle();
		}
	}

	//highlite current mouse position
	if(hover){
		//draw background rect
		ofSetColor(timeline->getColors().backgroundColor);
		if (timeline->getIsFrameBased()) {
			text = ofToString(indexForScreenX(ofGetMouseX()));
		}
		else{
			//text = ofToString();
			text = timeline->formatTime(timeForScreenX(ofGetMouseX()));
		}
		
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
	
	//draw current frame
	int currentFrameX;
	if (timeline->getIsFrameBased()) {
		text = ofToString(timeline->getCurrentFrame());
		currentFrameX = screenXForIndex(timeline->getCurrentFrame());
	}
	else{
		//text = ofToString();
		text = timeline->formatTime(timeline->getCurrentTime());
		currentFrameX = screenXForTime(timeline->getCurrentTime());
	}
	
	textH = 10;
	textW = (text.size()+1)*7;
	
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
	
	//draw in/out point
	float inPointX = normalizedXtoScreenX(timeline->getInOutRange().min, zoomBounds);
	float outPointX = normalizedXtoScreenX(timeline->getInOutRange().max, zoomBounds);
	
	if(bounds.x < inPointX){
		ofSetColor(timeline->getColors().disabledColor,120);
		ofRect(bounds.x, bounds.y, inPointX - bounds.x, totalDrawRect.height);
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(inPointX, bounds.y, inPointX, bounds.y+totalDrawRect.height);
	}
	
	if(bounds.x+bounds.width > outPointX){
		ofSetColor(timeline->getColors().disabledColor,120);
		ofRect(outPointX, bounds.y, (bounds.x+bounds.width) - outPointX, totalDrawRect.height);	
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(outPointX, bounds.y, outPointX, bounds.y+totalDrawRect.height);
	}
	
	//draw bounds 
	ofNoFill();
	ofSetColor(200, 180, 40);
	ofRect(bounds);
		
	ofPopStyle();
}

void ofxTLTicker::setBPM(float newBpm){
	bpm = newBpm;
	hasBPM = true;
}

//250 bpm = 250/60 beats per second
//1 beat = 1/(250/60) seconds
//1/2 beat = (1/(250/60))/2 seconds = 0.12 seconds
void ofxTLTicker::getSnappingPoints(vector<float>& points){
	//for now just add
	if(!drawBPMGrid){
		updateBPMPoints();
	}
	for(int i = 0; i < bpmScreenPoints.size(); i++){
		points.push_back(bpmScreenPoints[i].screenX);
	}
}

void ofxTLTicker::updateBPMPoints(){
	
	bpmScreenPoints.clear();
	if(!timeline->getIsFrameBased()){
		double currentPoint = 0;
		double oneMeasure = 1.0/(bpm/60.);
		double halfMeasure = oneMeasure/2;
		double quarterMeasure = halfMeasure/2;
		
		bool showMeasure = false;
		bool showHalfMeasure = false;
		bool showQuarterMeasure = false;
		showMeasure = screenXForTime(oneMeasure) - screenXForTime(0) > 20;
		if(showMeasure){
			showHalfMeasure = screenXForTime(halfMeasure) - screenXForTime(0) > 20;
			if (showHalfMeasure) {
				showQuarterMeasure = screenXForTime(halfMeasure) - screenXForTime(0) > 20;
			}
		}
		
			
		while(currentPoint < timeline->getDurationInSeconds()){
			ofxTLBPMPoint measures[4];
			int numMeasures = 0;
			if(showMeasure){
				measures[0].screenX = screenXForTime(currentPoint);
				measures[0].weight = 4;
				numMeasures = 1;
			}
			if(showHalfMeasure){
				measures[1].screenX = screenXForTime(currentPoint+halfMeasure);
				measures[1].weight = 2;
				numMeasures = 2;
			}
			if(showQuarterMeasure){
				measures[2].screenX = screenXForTime(currentPoint+quarterMeasure);
				measures[2].weight = 1;
				measures[3].screenX = screenXForTime(currentPoint+halfMeasure+quarterMeasure);
				measures[3].weight = 1;
				numMeasures = 4;
			}
			
//			cout << "measures " << measures[0].screenX << endl;
			
			for(int m = 0; m < numMeasures; m++){
				if( isOnScreen(measures[m].screenX) ){
					bpmScreenPoints.push_back( measures[m] );
				}
			}

			currentPoint += oneMeasure;
		}
	}	
}

bool ofxTLTicker::getDrawBPMGrid(){
	return drawBPMGrid;
}

void ofxTLTicker::setDrawBPMGrid(bool drawGrid){
	drawBPMGrid = drawGrid;
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
		timeline->setCurrentFrame(indexForScreenX(ofGetMouseX()));
	}
	else{
		timeline->setCurrentTime(timeForScreenX(ofGetMouseX()));
	}
	
}