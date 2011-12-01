/*
 *  ofxTLElement.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLElement.h"
#include "ofxTimeline.h"

ofxTLElement::ofxTLElement()
:	xmlFileName(""),
	bounds(ofRectangle(0,0,0,0)),
	zoomBounds(ofRange(0,1.0)),
	enabled(true),
	focused(false),
	hover(false),
	createdByTimeline(false),
	autosave(true),
	timeline(NULL)
{
	//
}

ofxTLElement::~ofxTLElement(){
	disable();
}

void ofxTLElement::enable(){
	enabled = true;
	ofxTLRegisterZoomEvents(this);
}

void ofxTLElement::disable(){
	enabled = false;
	focused = false;
	ofxTLRemoveZoomEvents(this);	
}

void ofxTLElement::setDrawRect(ofRectangle drawRect){
	bounds = drawRect;
	drawRectChanged();
}

void ofxTLElement::offsetDrawRect(ofVec2f offset){
	bounds.x += offset.x;
	bounds.y += offset.y;
	drawRectChanged();
}

ofxTimeline* ofxTLElement::getTimeline(){
	return timeline;
}
	
void ofxTLElement::setTimeline(ofxTimeline* _timeline){
	timeline = _timeline;
}

void ofxTLElement::mouseMoved(ofMouseEventArgs& args){
	hover = bounds.inside(args.x, args.y);
}

void ofxTLElement::mousePressed(ofMouseEventArgs& args){
	focused = bounds.inside(args.x, args.y);
}

ofRectangle ofxTLElement::getDrawRect(){
	return bounds;
}

bool ofxTLElement::getCreatedByTimeline(){
	return createdByTimeline;
}

void ofxTLElement::setCreatedByTimeline(bool created){
	createdByTimeline = created;
}

void ofxTLElement::zoomStarted(ofxTLZoomEventArgs& args){
	setZoomBounds(args.currentZoom);
}

void ofxTLElement::zoomDragged(ofxTLZoomEventArgs& args){
	setZoomBounds(args.currentZoom);
}

void ofxTLElement::zoomEnded(ofxTLZoomEventArgs& args){
	setZoomBounds(args.currentZoom);
}

void ofxTLElement::setZoomBounds(ofRange zoomBoundsPercent){
	zoomBounds = zoomBoundsPercent;
}

void ofxTLElement::setXMLFileName(string filename){
	xmlFileName = filename;
}

void ofxTLElement::setAutosave(bool _autosave){
	autosave = _autosave;
}
			   
bool ofxTLElement::hasFocus(){
	return focused;
}

bool ofxTLElement::pointInScreenBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

float ofxTLElement::screenXtoNormalizedX(float x){
	return ofMap(x, bounds.x, bounds.x+bounds.width, 0.0, 1.0, true);
}

float ofxTLElement::normalizedXtoScreenX(float x){
	return ofMap(x, 0.0, 1.0, bounds.x, bounds.x+bounds.width, true);
}

int ofxTLElement::indexForScreenX(int screenX){
	return indexForScreenX(screenX, timeline->getDurationInFrames());
}

int ofxTLElement::screenXForIndex(int index){
	return screenXForIndex(index, timeline->getDurationInFrames());	
}

int ofxTLElement::indexForScreenX(int screenX, int durationInFrames){
	int startFrame = zoomBounds.min * durationInFrames;
	int endFrame = zoomBounds.max * durationInFrames;
	return ofMap(screenX, bounds.x, bounds.x+bounds.width, startFrame, endFrame, true);
}

int ofxTLElement::screenXForIndex(int index, int durationInFrames){
	int startFrame = zoomBounds.min * durationInFrames;
	int endFrame = zoomBounds.max * durationInFrames;
	return ofMap(index, startFrame, endFrame, bounds.x, bounds.x+bounds.width, false);
}


