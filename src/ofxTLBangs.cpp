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
#include "ofxTLBangs.h"

ofxTLBangs::ofxTLBangs(){
    lastTimelinePoint = 0;
	lastBangTime = 0;
}

ofxTLBangs::~ofxTLBangs(){
	disable();
}

void ofxTLBangs::draw(){
        
    if(bounds.height < 2){
        return;
    }
    
    ofPushStyle();
    ofFill();
	
	//float currentPercent = powf(MIN(ofGetElapsedTimef() - lastBangTime, .5), 2);
	float currentPercent = powf(ofMap(ofGetElapsedTimef() - lastBangTime, 0, .5, 1.0, 0,true), 2);
	if(currentPercent > 0){
		ofSetColor(timeline->getColors().disabledColor, 100*(currentPercent));
		ofFill();
		ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	}
	
    for(int i = keyframes.size()-1; i >= 0; i--){
		if(!isKeyframeIsInBounds(keyframes[i])){
			continue;
		}
        //int screenX = normalizedXtoScreenX(keyframes[i]->position.x);
        int screenX = millisToScreenX(keyframes[i]->time);
        if(isKeyframeSelected(keyframes[i])){
            ofSetLineWidth(2);
            ofSetColor(timeline->getColors().textColor);
        }
        else if(keyframes[i] == hoverKeyframe){
            ofSetLineWidth(4);
            ofSetColor(timeline->getColors().highlightColor);
        }
        else{
            ofSetLineWidth(4);
            ofSetColor(timeline->getColors().keyColor);
        }
        
        ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
    }
    ofPopStyle();

}

void ofxTLBangs::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
    	if(timeRange.contains( keyframes[i]->time )){
            selectKeyframe(keyframes[i]);
        }
	}
}

ofxTLKeyframe* ofxTLBangs::keyframeAtScreenpoint(ofVec2f p){
    if(bounds.inside(p.x, p.y)){
        for(int i = 0; i < keyframes.size(); i++){
            float offset = p.x - timeline->millisToScreenX(keyframes[i]->time);            
            if (abs(offset) < 5) {
                return keyframes[i];
            }
        }
    }
	return NULL;    
}

void ofxTLBangs::update(){
//	if(isPlaying || timeline->getIsPlaying()){
		long thisTimelinePoint = currentTrackTime();
		for(int i = 0; i < keyframes.size(); i++){
			if(timeline->getInOutRangeMillis().contains(keyframes[i]->time) &&
               lastTimelinePoint <= keyframes[i]->time &&
               thisTimelinePoint >= keyframes[i]->time &&
               thisTimelinePoint != lastTimelinePoint)
            {
//				ofLogNotice() << "fired bang with accuracy of " << (keyframes[i]->time - thisTimelinePoint) << endl;
				bangFired(keyframes[i]);
				lastBangTime = ofGetElapsedTimef();
			}
		}
		lastTimelinePoint = thisTimelinePoint;
//	}
}

void ofxTLBangs::bangFired(ofxTLKeyframe* key){
    ofxTLBangEventArgs args;
    args.sender = timeline;
    args.track = this;
	//play solo change
    //args.currentMillis = timeline->getCurrentTimeMillis();
	args.currentMillis = currentTrackTime();
    args.currentPercent = timeline->getPercentComplete();
    args.currentFrame = timeline->getCurrentFrame();
    args.currentTime = timeline->getCurrentTime();
    ofNotifyEvent(events().bangFired, args);    
}

void ofxTLBangs::playbackStarted(ofxTLPlaybackEventArgs& args){
	ofxTLTrack::playbackStarted(args);
	lastTimelinePoint = currentTrackTime();
}

void ofxTLBangs::playbackEnded(ofxTLPlaybackEventArgs& args){
//    isPlayingBack = false;
}

void ofxTLBangs::playbackLooped(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = 0;
}

string ofxTLBangs::getTrackType(){
    return "Bangs";
}
