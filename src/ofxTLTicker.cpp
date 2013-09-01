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

#include "ofxTLTicker.h"
#include "ofxTimeline.h"

ofxTLTicker::ofxTLTicker() {
	dragging = false;
    hasBPM = false;
	drawBPMGrid = false;
	hoverTime = 0;
    playOnMouseReleased = false;
}

ofxTLTicker::~ofxTLTicker(){
}

void ofxTLTicker::draw(){
	
	if(bounds.height == 0){
		return;
	}
	
	ofPushStyle();
	
	int textH, textW;
	string text;

    //draw tickers with time
    float startTime = zoomBounds.min * timeline->getDurationInSeconds();
    float endTime = zoomBounds.max * timeline->getDurationInSeconds();
    float durationInview = endTime-startTime;
    float secondsPerPixel = durationInview / bounds.width;
    
	if(viewIsDirty){
		refreshTickMarks();
	}
	
	tickerMarks.setStrokeColor( ofColor(200, 180, 40) );
	tickerMarks.setStrokeWidth(1);
	tickerMarks.draw(0, bounds.y);
		
    if(drawBPMGrid){
		if(viewIsDirty){
	        updateBPMPoints();
		}
        ofPushStyle();
        ofSetColor(255, 255, 255, 50);
        for(int i = 0; i < bpmScreenPoints.size(); i++){
            ofSetLineWidth(bpmScreenPoints[i].weight);
            ofLine(bpmScreenPoints[i].screenX, getBottomEdge(), bpmScreenPoints[i].screenX, totalDrawRect.y+totalDrawRect.height);
        }
        ofPopStyle();
    }

	textH = timeline->getFont().getLineHeight();
	textW = 3;

	//highlite current mouse position
	if(hover){
		//draw background rect
		ofSetColor(timeline->getColors().backgroundColor);
        float screenX = ofClamp(millisToScreenX(hoverTime), bounds.getMinX(), bounds.getMaxX());
		text = timeline->formatTime(hoverTime);
		textW = timeline->getFont().stringWidth(text)+3;
        if(bounds.height > 2){
//            int previewTimecodeX = ofClamp(screenX+5, bounds.getMinX(), bounds.getMaxX()-textW-5);
			int previewTimecodeX;
			if (timeline->getIsFrameBased()) {
				text = ofToString( timeline->getTimecode().frameForMillis(hoverTime) );
			}
			else{
				text = timeline->formatTime(hoverTime);
			}
			previewTimecodeX = ofClamp(screenX+5, bounds.getMinX(), bounds.getMaxX()-textW-5);
            ofFill();
            ofRect(previewTimecodeX-5, bounds.y+textH, textW, textH);		
            //draw playhead line
            ofSetColor(timeline->getColors().textColor);
            timeline->getFont().drawString(text, previewTimecodeX, bounds.y+textH*2);
        }
		
		ofSetColor(timeline->getColors().highlightColor);
		ofSetLineWidth(1);

        ofLine(screenX, totalDrawRect.y, screenX, totalDrawRect.y+totalDrawRect.height);
	}
	
	//draw current frame
    int currentFrameX;
    if (timeline->getIsFrameBased()) {
        text = ofToString(timeline->getCurrentFrame());
        currentFrameX = screenXForTime( timeline->getTimecode().secondsForFrame(timeline->getCurrentFrame()));
    }
    else{
        text = timeline->formatTime(timeline->getCurrentTime());
        currentFrameX = screenXForTime(timeline->getCurrentTime());
    }
    currentFrameX = ofClamp(currentFrameX, bounds.getMinX(), bounds.getMaxX());
							
    if(bounds.height > 2){
        int timeCodeX = ofClamp(currentFrameX+5, bounds.getMinX(), bounds.getMaxX()-textW-5);
        ofSetColor(timeline->getColors().backgroundColor);
        ofFill();
        ofRect(timeCodeX-5, bounds.y, textW, textH);
        ofSetColor(timeline->getColors().textColor);
        timeline->getFont().drawString(text, timeCodeX, bounds.y+textH);
    }
	
    if(timeline->getIsPlaying()){
        ofSetColor(timeline->getColors().keyColor);
    }
    else{
        ofSetColor(timeline->getColors().outlineColor);
    }
	
	//draw playhead line
	ofSetLineWidth(1);
	ofLine(currentFrameX, totalDrawRect.y, currentFrameX, totalDrawRect.y+totalDrawRect.height);
	//draw bounds 
	ofNoFill();
	ofSetColor(200, 180, 40);
	ofRect(bounds);
		
	ofPopStyle();
}

void ofxTLTicker::setHoverTime(unsigned long long millis){
    hoverTime = millis;
}

void ofxTLTicker::setBPM(float newBpm){
	bpm = newBpm;
	hasBPM = true;
}

float ofxTLTicker::getBPM(){
	return bpm;
}

//250 bpm = 250/60 beats per second
//1 beat = 1/(250/60) seconds
//1/2 beat = (1/(250/60))/2 seconds = 0.12 seconds
void ofxTLTicker::getSnappingPoints(set<unsigned long long>& points){

	if(!drawBPMGrid){
		updateBPMPoints();
	}
    
	for(int i = 0; i < bpmScreenPoints.size(); i++){
		points.insert(bpmScreenPoints[i].millis);
	}
	
	points.insert(timeline->getCurrentTimeMillis());
}

void ofxTLTicker::refreshTickMarks(){
	tickerMarks.clear();

    unsigned long long startMillis = zoomBounds.min * timeline->getDurationInMilliseconds();
    unsigned long long endMillis = zoomBounds.max * timeline->getDurationInMilliseconds();
    unsigned long long durationInview = endMillis-startMillis;
    float millisPerPixel = durationInview / bounds.width;
	
	//expand to days
	bool showMillis;
	bool showSeconds;
	bool showMinutes;
	int step = 4;
	//find the scale of time being shown
	if(millisPerPixel > 1000*60 * step){ //each pixel is more than a minute
		showMillis = false;
		showSeconds = false;
		showMinutes = false;
	}
	else if(millisPerPixel > 1000 * step){ //each pixel is more than a second
		showMillis = false;
		showSeconds = false;
		showMinutes = true;
	}
	else if(millisPerPixel > step){ //each pixel is more than a millisecond
		showMillis = false;
		showSeconds = true;
		showMinutes = true;
	}
	else{ //each pixel is less than a millsecond
		showMillis = true;
		showSeconds = true;
		showMinutes = true;
	}
	
	unsigned long long lastMillis = screenXToMillis(bounds.x);
	int lastSecond = lastMillis/1000;
	int lastMinute = lastSecond/60;
	int lastHour = lastMinute/60;
	for(int i = bounds.getMinX()+step; i < bounds.getMaxX(); i+=step){
		int height = 0;
		unsigned long long currentMillis = screenXToMillis(i);
		int currentSecond = currentMillis/1000;
		int currentMinute = currentSecond/60;
		int currentHour = currentMinute/60;
		float x;
		if(showMillis && currentMillis > lastMillis){
			height = bounds.height*.25;
			lastMillis = currentMillis;
			x = millisToScreenX(currentMillis);
		}

		if(showSeconds && currentSecond > lastSecond){
			height = bounds.height*.5;
			lastSecond = currentSecond;
			x = millisToScreenX(lastSecond*1000);
		}
				
		if(showMinutes && currentMinute > lastMinute){
			height = bounds.height*.75;
			lastMinute = currentMinute;
			x = millisToScreenX(lastMinute*1000*60);
		}
		
		if(currentHour > lastHour){
			height = bounds.height;
			lastHour = currentHour;
			x = millisToScreenX(lastMinute*1000*60*60);
		}
		
		if(height != 0){
			tickerMarks.moveTo(x, bounds.height - height);
			tickerMarks.lineTo(x, bounds.height);
		}
	}

}

//TODO: find a way to make this not happen every frame
void ofxTLTicker::updateBPMPoints(){
	
	bpmScreenPoints.clear();

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

	int currentMeasure = 0;        
	while(currentPoint < timeline->getDurationInSeconds()){
		ofxTLBPMPoint measures[4];
		int numMeasures = 0;
		if(showMeasure){
			measures[0].millis = currentPoint * 1000;
			measures[0].screenX = millisToScreenX(measures[0].millis);
			measures[0].weight = 4;
			numMeasures = 1;
		}
		if(showHalfMeasure){
			measures[1].millis = (currentPoint+halfMeasure) * 1000;
			measures[1].screenX = millisToScreenX(measures[1].millis);
			measures[1].weight = 2;
			numMeasures = 2;
		}
		if(showQuarterMeasure){
			measures[2].millis = (currentPoint+quarterMeasure) * 1000;
			measures[2].screenX = millisToScreenX(measures[2].millis);
			measures[2].weight = 1;
			measures[3].millis = (currentPoint+halfMeasure+quarterMeasure) * 1000;
			measures[3].screenX = millisToScreenX(measures[3].millis);
			measures[3].weight = 1;
			numMeasures = 4;
		}
		
		for(int m = 0; m < numMeasures; m++){
			if( isOnScreen(measures[m].screenX) ){
				bpmScreenPoints.push_back( measures[m] );
			}
		}

		currentMeasure++;
		currentPoint = currentMeasure*oneMeasure;
	}

}

bool ofxTLTicker::getIsScrubbing(){
	return playOnMouseReleased;
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

	dragging = bounds.inside(args.x, args.y);
	if(dragging){
        if(timeline->getIsPlaying()){
            playOnMouseReleased = true;
            timeline->stop();
        }
		updateTimelinePosition();
        ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(events().playheadScrubbed, args);		
	}
}

void ofxTLTicker::mouseDragged(ofMouseEventArgs& args){
	if(dragging){
		updateTimelinePosition();
        ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(events().playheadScrubbed, args);
	}	
}

void ofxTLTicker::mouseReleased(ofMouseEventArgs& args){
    if(dragging){
		ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(events().playheadScrubbed, args);
        if(playOnMouseReleased){
            timeline->play();
            playOnMouseReleased = false;
        }
    }

	dragging = false;
}

void ofxTLTicker::setTotalDrawRect(ofRectangle drawRect){
	totalDrawRect = drawRect;
}

void ofxTLTicker::updateTimelinePosition(){
	timeline->setCurrentTimeSeconds(timeForScreenX(ofGetMouseX()));
}
