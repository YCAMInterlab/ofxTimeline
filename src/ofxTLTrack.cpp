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

#include "ofxTLTrack.h"
#include "ofxTimeline.h"

ofxTLTrack::ofxTLTrack()
:	xmlFileName(""),
	bounds(ofRectangle(0,0,0,0)),
	zoomBounds(ofRange(0,1.0)),
	enabled(true),
	focused(false),
	active(false),
	hover(false),
	createdByTimeline(false),
	autosave(true),
	timeline(NULL)
{
	//
}

ofxTLTrack::~ofxTLTrack(){
	disable();
}

void ofxTLTrack::enable(){
	enabled = true;
	ofxTLRegisterZoomEvents(this);
}

void ofxTLTrack::disable(){
	enabled = false;
	focused = false;
	ofxTLRemoveZoomEvents(this);	
}

void ofxTLTrack::setDrawRect(ofRectangle drawRect){
	bounds = drawRect;
	drawRectChanged();
}

void ofxTLTrack::offsetDrawRect(ofVec2f offset){
	bounds.x += offset.x;
	bounds.y += offset.y;
	drawRectChanged();
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

string ofxTLTrack::getName(){
	return name;
}

bool ofxTLTrack::_mousePressed(ofMouseEventArgs& args){
    focused = bounds.inside(args.x, args.y);
    mousePressed(args);
    active = focused;
    return focused;
}

void ofxTLTrack::_mouseMoved(ofMouseEventArgs& args){
	hover = bounds.inside(args.x, args.y);
	mouseMoved(args);    
}

void ofxTLTrack::_mouseReleased(ofMouseEventArgs& args){
    mouseReleased(args);
    active = false;
}

ofRectangle ofxTLTrack::getDrawRect(){
	return bounds;
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

void ofxTLTrack::setAutosave(bool _autosave){
	autosave = _autosave;
}
			   
bool ofxTLTrack::hasFocus(){
	return focused;
}

bool ofxTLTrack::isEnabled(){
	return enabled;
}

bool ofxTLTrack::pointInScreenBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

float ofxTLTrack::screenXtoNormalizedX(float x){
	return ofMap(x, bounds.x, bounds.x+bounds.width, 0.0, 1.0, true);
}

float ofxTLTrack::normalizedXtoScreenX(float x){
	return ofMap(x, 0.0, 1.0, bounds.x, bounds.x+bounds.width, true);
}

float ofxTLTrack::screenXtoNormalizedX(float x, ofRange outputRange){
	return ofMap(x, bounds.x, bounds.x+bounds.width, outputRange.min, outputRange.max, false);
}

float ofxTLTrack::normalizedXtoScreenX(float x, ofRange inputRange){
	return ofMap(x, inputRange.min, inputRange.max, bounds.x, bounds.x+bounds.width, false);
}


int ofxTLTrack::indexForScreenX(int screenX){
	return indexForScreenX(screenX, timeline->getDurationInFrames());
}

int ofxTLTrack::screenXForIndex(int index){
	return screenXForIndex(index, timeline->getDurationInFrames());	
}

int ofxTLTrack::indexForScreenX(int screenX, int durationInFrames){
	int startFrame = zoomBounds.min * durationInFrames;
	int endFrame = zoomBounds.max * durationInFrames;
	return ofMap(screenX, bounds.x, bounds.x+bounds.width, startFrame, endFrame, true);
}

int ofxTLTrack::screenXForIndex(int index, int durationInFrames){
	int startFrame = zoomBounds.min * durationInFrames;
	int endFrame = zoomBounds.max * durationInFrames;
	return ofMap(index, startFrame, endFrame, bounds.x, bounds.x+bounds.width, false);
}

float ofxTLTrack::screenXForTime(float time){
	return screenXForTime(time, timeline->getDurationInSeconds());
}

float ofxTLTrack::timeForScreenX(float screenX){
	return timeForScreenX(screenX, timeline->getDurationInSeconds());
}

float ofxTLTrack::screenXForTime(float time, float durationInSeconds){
	float startTime = zoomBounds.min * durationInSeconds;
	float endTime = zoomBounds.max *durationInSeconds;
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