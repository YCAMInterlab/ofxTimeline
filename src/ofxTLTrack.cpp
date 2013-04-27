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

#include "ofxTLTrack.h"
#include "ofxTimeline.h"

ofxTLTrack::ofxTLTrack()
:	xmlFileName(""),
	bounds(ofRectangle(0,0,0,0)),
	zoomBounds(ofRange(0,1.0)),
	enabled(false),
	focused(false),
	active(false),
	hover(false),
	createdByTimeline(false),
	timeline(NULL),
	playbackStartTime(0),
	isPlaying(false)
{

}

ofxTLTrack::~ofxTLTrack(){
    if(isEnabled()){
		disable();
    }
}

void ofxTLTrack::setup(){
    enable();
    load();
}

void ofxTLTrack::enable(){
    if(!enabled){
		enabled = true;
		events().registerZoomEvents(this);
		events().registerPlaybackEvents(this);
    }
}

void ofxTLTrack::disable(){
    if(enabled){
		enabled = false;
		focused = false;
		events().removeZoomEvents(this);
		events().removePlaybackEvents(this);
    }
}

void ofxTLTrack::setDrawRect(ofRectangle drawRect){
	if(drawRect != bounds){
		bounds = drawRect;
		viewIsDirty = true;
		drawRectChanged();
	}
}

ofxTimeline* ofxTLTrack::getTimeline(){
	return timeline;
}
	
void ofxTLTrack::setTimeline(ofxTimeline* _timeline){
	timeline = _timeline;
}

void ofxTLTrack::setName(string _name){
	name = _name;
}

//optional display name that track headers will use
void ofxTLTrack::setDisplayName(string name){
    displayName = name;
}

string ofxTLTrack::getDisplayName(){
    return displayName == "" ? name : displayName;
}

string ofxTLTrack::getTrackType(){
	return "Track";    
}

string ofxTLTrack::getName(){
	return name;
}

void ofxTLTrack::_draw(){
	ofPushStyle();
	
	if(focused){
		ofFill();
		ofSetColor(timeline->getColors().highlightColor, 50);
		ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	}
	
	ofNoFill();
	if(hover){
		ofSetColor(timeline->getColors().textColor);
	}
	else{
		ofSetColor(timeline->getColors().outlineColor);
	}	
	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	ofPopStyle();

	ofPushStyle();
    draw();
	ofPopStyle();
	
	if(isPlaying){
		float playheadScreenX = millisToScreenX(currentTrackTime());
		if(isOnScreen(playheadScreenX)){
			ofPushStyle();
			ofSetColor(timeline->getColors().keyColor);
			ofLine(playheadScreenX, bounds.getMinY(), playheadScreenX, bounds.getMaxY());
			ofPopStyle();
		}			
	}
	viewIsDirty = false;
}

unsigned long long ofxTLTrack::currentTrackTime(){
	if(isPlaying){
		currentTime = timeline->getTimer().getAppTimeMillis() - playbackStartTime;
		checkLoop();
		return currentTime;
		//return timeline->getInTimeInMillis() + (timeline->getTimer().getAppTimeMillis() - playbackStartTime) % timeline->getInOutRangeMillis().span() ;
	}
	else{
		return timeline->getCurrentTimeMillis();
	}
}

void ofxTLTrack::checkLoop(){
	if(currentTime < timeline->getInTimeInMillis()){
        currentTime = timeline->getInTimeInMillis();
        playbackStartTime = timeline->getTimer().getAppTimeSeconds() - currentTime;
//        playbackStartFrame = ofGetFrameNum() - timecode.frameForSeconds(currentTime);
    }
    
    if(currentTime >= timeline->getOutTimeInMillis()){
        if(timeline->getLoopType() == OF_LOOP_NONE){
            stop();
        }
        else if(timeline->getLoopType() == OF_LOOP_NORMAL) {
            currentTime = timeline->getInTimeInMillis() + (currentTime - timeline->getOutTimeInMillis());
//            playbackStartFrame += getDurationInFrames()  * inoutRange.span();
            playbackStartTime  += timeline->getInOutRangeMillis().span();
        }
    }
}
bool ofxTLTrack::togglePlay(){
	if(isPlaying){
		stop();
	}
	else{
		play();
	}
	return isPlaying;
}

void ofxTLTrack::play(){
	if(!isPlaying && !timeline->getIsPlaying()){
		isPlaying = true;
		currentTime = ofClamp(timeline->getCurrentTimeMillis(), timeline->getInTimeInMillis(), timeline->getOutTimeInMillis());
		playbackStartTime = timeline->getTimer().getAppTimeMillis() - currentTime;
		checkLoop();
	}
}

void ofxTLTrack::stop(){
	if(isPlaying){
		isPlaying = false;
	}
}

bool ofxTLTrack::getIsPlaying(){
	return isPlaying;
}

void ofxTLTrack::playbackStarted(ofxTLPlaybackEventArgs& args){
	//we stop playing solo if the main timeline starts
	if(timeline->getTimecontrolTrack() == NULL || this != timeline->getTimecontrolTrack()){
		stop();
	}
}

bool ofxTLTrack::_mousePressed(ofMouseEventArgs& args, long millis){
    if(enabled){
	    active = bounds.inside(args.x, args.y);
    	return mousePressed(args, millis);
    }
	return false;
}

void ofxTLTrack::_mouseMoved(ofMouseEventArgs& args, long millis){
    if(enabled){
    	hover = bounds.inside(args.x, args.y);
		mouseMoved(args, millis);
    }
}

void ofxTLTrack::_mouseDragged(ofMouseEventArgs& args, long millis){
    if(enabled){
    	mouseDragged(args, millis);
    }
}

void ofxTLTrack::_mouseReleased(ofMouseEventArgs& args, long millis){
    if(enabled){
        mouseReleased(args, millis);
	    active = false;
    }
}

void ofxTLTrack::gainedFocus(){
	focused = true;
}

void ofxTLTrack::lostFocus(){
    focused = false;
}

ofRectangle ofxTLTrack::getDrawRect(){
	return bounds;
}

float ofxTLTrack::getBottomEdge(){
	return bounds.getMaxY();
}

bool ofxTLTrack::getCreatedByTimeline(){
	return createdByTimeline;
}

void ofxTLTrack::setCreatedByTimeline(bool created){
	createdByTimeline = created;
}

void ofxTLTrack::zoomStarted(ofxTLZoomEventArgs& args){
	setZoomBounds(args.currentZoom);
}

void ofxTLTrack::zoomDragged(ofxTLZoomEventArgs& args){
	setZoomBounds(args.currentZoom);
}

void ofxTLTrack::zoomEnded(ofxTLZoomEventArgs& args){
	setZoomBounds(args.currentZoom);
}

void ofxTLTrack::setZoomBounds(ofRange zoomBoundsPercent){
	zoomBounds = zoomBoundsPercent;
	viewIsDirty = true;
}

void ofxTLTrack::setXMLFileName(string filename){
	xmlFileName = filename;
}

string ofxTLTrack::getXMLFilePath(){
	return xmlFileName;	
}

string ofxTLTrack::getXMLFileName(){
	return ofFilePath::getFileName(xmlFileName);
}

ofxTLEvents& ofxTLTrack::events(){
	return timeline->events();    
}

bool ofxTLTrack::isActive(){
	return active;    
}

bool ofxTLTrack::hasFocus(){
	return focused;
}

bool ofxTLTrack::isEnabled(){
	return enabled;
}

bool ofxTLTrack::isHovering(){
	return hover;
}

bool ofxTLTrack::pointInScreenBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

float ofxTLTrack::screenXtoNormalizedX(float x){
    return timeline->screenXtoNormalizedX(x);
}

long ofxTLTrack::screenXToMillis(float x){
    return timeline->screenXToMillis(x);
}

float ofxTLTrack::millisToScreenX(long millis){
    return timeline->millisToScreenX(millis);
}

float ofxTLTrack::normalizedXtoScreenX(float x){
    return timeline->normalizedXtoScreenX(x);
}

float ofxTLTrack::screenXtoNormalizedX(float x, ofRange outputRange){
    return timeline->screenXtoNormalizedX(x, outputRange);
}

float ofxTLTrack::normalizedXtoScreenX(float x, ofRange inputRange){
    return timeline->normalizedXtoScreenX(x, inputRange);
}

//int ofxTLTrack::indexForScreenX(int screenX){
//	return indexForScreenX(screenX, timeline->getDurationInFrames());
//}

//int ofxTLTrack::screenXForIndex(int index){
//	return screenXForIndex(index, timeline->getDurationInFrames());	
//}

//int ofxTLTrack::indexForScreenX(int screenX, int durationInFrames){
//	int startFrame = zoomBounds.min * durationInFrames;
//	int endFrame = zoomBounds.max * durationInFrames;
//	return ofMap(screenX, bounds.x, bounds.x+bounds.width, startFrame, endFrame, true);
//}
//
//int ofxTLTrack::screenXForIndex(int index, int durationInFrames){
//	int startFrame = zoomBounds.min * durationInFrames;
//	int endFrame = zoomBounds.max * durationInFrames;
//	return ofMap(index, startFrame, endFrame, bounds.x, bounds.x+bounds.width, false);
//}

float ofxTLTrack::screenXForTime(float time){
	return screenXForTime(time, timeline->getDurationInSeconds());
}

float ofxTLTrack::timeForScreenX(float screenX){
	return timeForScreenX(screenX, timeline->getDurationInSeconds());
}

float ofxTLTrack::screenXForTime(float time, float durationInSeconds){
	float startTime = zoomBounds.min * durationInSeconds;
	float endTime = zoomBounds.max * durationInSeconds;
	return ofMap(time, startTime, endTime, bounds.x, bounds.x+bounds.width, false);
}

float ofxTLTrack::timeForScreenX(float screenX, float durationInSeconds){
	float startTime = zoomBounds.min * durationInSeconds;
	float endTime = zoomBounds.max *durationInSeconds;
	return ofMap(screenX, bounds.x, bounds.x+bounds.width, startTime, endTime, true);	
}

bool ofxTLTrack::isOnScreen(float screenX){
	return screenX > bounds.x && screenX < bounds.x+bounds.width;
}