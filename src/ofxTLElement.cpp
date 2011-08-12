/*
 *  ofxTLElement.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLElement.h"
#include "ofxTimelineUtils.h"

ofxTLElement::ofxTLElement()
:	xmlFileName(""),
	bounds(ofRectangle(0,0,0,0)),
	zoomBounds(ofRange(0,1.0)),
	enabled(true),
	focused(false),
	hover(false)
{
	enable();
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
}

void ofxTLElement::offsetDrawRect(ofVec2f offset){
	bounds.x += offset.x;
	bounds.y += offset.y;
}

ofRectangle ofxTLElement::getDrawRect(){
	return bounds;
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
	if(autosave) save();
}

void ofxTLElement::setAutosave(bool _autosave){
	autosave = _autosave;
	if(autosave) save();
}
			   
bool ofxTLElement::hasFocus()
{
	return focused;
}

bool ofxTLElement::pointInScreenBounds(ofVec2f screenpoint)
{
	return isPointInRect(screenpoint, bounds);
}

float ofxTLElement::screenXtoNormalizedX(float x)
{
	return ofMap(x, bounds.x, bounds.x+bounds.width, 0.0, 1.0, true);
}

float ofxTLElement::normalizedXtoScreenX(float x)
{
	return ofMap(x, 0.0, 1.0, bounds.x, bounds.x+bounds.width, true);
}
