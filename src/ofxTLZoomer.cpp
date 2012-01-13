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

#include "ofxTLZoomer.h"
#include "ofxXmlSettings.h"
#include "ofRange.h"
#include "ofxTimeline.h"

ofxTLZoomer::ofxTLZoomer()
:	minSelected(false),
	maxSelected(false),
	midSelected(false),
	currentViewRange(ofRange(0.0, 1.0))
{
	//default constructor
}

ofxTLZoomer::~ofxTLZoomer(){
	
}

void ofxTLZoomer::setup(){
	enable();
	load();
}

void ofxTLZoomer::draw(){
	ofPushStyle();
	ofEnableSmoothing();

	ofNoFill();
	if(focused){
		ofSetColor(255, 200, 0); //focused outline color
	}
	else{
		ofSetColor(150, 150, 0); //unfocused outline color
	}
	
	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);

	//draw min
	float screenY = bounds.y + bounds.height/2.0;
	float minScreenX = normalizedXtoScreenX(currentViewRange.min);
	float maxScreenX = normalizedXtoScreenX(currentViewRange.max);

	if(midSelected){
		ofSetLineWidth(2);
	}
	else{
		ofSetLineWidth(1);
	}

	ofLine(minScreenX, screenY, maxScreenX, screenY);
	ofSetLineWidth(1);

	if(minSelected){
		ofFill();
	}
	else{
		ofNoFill();
	}
	ofCircle(minScreenX, screenY, 5);

	if(maxSelected){
		ofFill();
	}
	else{
		ofNoFill();
	}
	ofCircle(maxScreenX, screenY, 5);

	//draw playhead reference
	ofSetColor(timeline->getColors().keyColor);
	ofLine(bounds.x+bounds.width*timeline->getPercentComplete(), bounds.y, bounds.x+bounds.width*timeline->getPercentComplete(), bounds.y+bounds.height);
	ofPopStyle();
}

void ofxTLZoomer::load() {
	notifyZoomStarted();
	
	ofxXmlSettings settings;
	if(!settings.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "ofxTLZoomer -- couldn't load zoom settings file");
		return;
	}

	settings.pushTag("zoom");
	currentViewRange = ofRange(settings.getValue("min", 0.0),
							   settings.getValue("max", 1.0));

	settings.popTag();
	
	notifyZoomEnded();
}

void ofxTLZoomer::save() {
	ofxXmlSettings savedSettings;
	savedSettings.addTag("zoom");
	savedSettings.pushTag("zoom");
	savedSettings.addValue("min", currentViewRange.min);
	savedSettings.addValue("max", currentViewRange.max);
	savedSettings.popTag();//zoom
	savedSettings.saveFile(xmlFileName);
}

void ofxTLZoomer::mouseMoved(ofMouseEventArgs& args) {
	
}

void ofxTLZoomer::mousePressed(ofMouseEventArgs& args) {

	if(!enabled) return;

	minSelected = maxSelected = midSelected = focused  = false;
	if (pointInScreenBounds(ofVec2f(args.x, args.y))) {
		mouseIsDown = true;
		focused = true;
		
		//did we click on the min-left handle?
		float minScreenX = normalizedXtoScreenX(currentViewRange.min);
		minGrabOffset = args.x - minScreenX;
		if(fabs(minScreenX - args.x) < 5){
			minSelected = true;
			notifyZoomStarted();
			return;
		}
		
		//did we click on the max-right handle?
		float maxScreenX = normalizedXtoScreenX(currentViewRange.max);
		maxGrabOffset = args.x - maxScreenX;
		if(fabs(maxScreenX - args.x) < 5){
			maxSelected = true;
			notifyZoomStarted();
			return;
		}
		
		//did we click in the middle?
		if(args.x > minScreenX && args.x < maxScreenX){
			notifyZoomStarted();
			midSelected = true;
			return;
		}
		
		//did we click to the right?
		if(args.x > maxScreenX){
			maxSelected = true;
			maxGrabOffset = 0;
			currentViewRange.max = screenXtoNormalizedX(args.x);
			notifyZoomStarted();
			return;
		}
		
		//did we click to the left?
		if(args.x < minScreenX){
			minSelected = true;
			minGrabOffset = 0;
			currentViewRange.min = screenXtoNormalizedX(args.x);
			notifyZoomStarted();
			return;
		}
	}
}

void ofxTLZoomer::mouseDragged(ofMouseEventArgs& args) {
	if(!enabled) return;

	ofRange oldRange = currentViewRange;
	if(minSelected || midSelected){
		currentViewRange.min = ofClamp( screenXtoNormalizedX(args.x-minGrabOffset), 0, currentViewRange.max-.0001);
		notifyZoomDragged(oldRange);
	}

	if(maxSelected || midSelected){
		currentViewRange.max = ofClamp( screenXtoNormalizedX(args.x-maxGrabOffset), currentViewRange.min+.0001, 1.0);
		notifyZoomDragged(oldRange);
	}	
}

bool ofxTLZoomer::isActive(){
	return mouseIsDown && ( maxSelected || minSelected || midSelected);
}

void ofxTLZoomer::mouseReleased(ofMouseEventArgs& args){
	if(!enabled) return;
	
	if(mouseIsDown){
		mouseIsDown = false;
		notifyZoomEnded();
		if(autosave){
			save();
		}		
	}
}

void ofxTLZoomer::notifyZoomStarted(){
	ofxTLZoomEventArgs zoomEvent;
	zoomEvent.currentZoom = zoomEvent.oldZoom = currentViewRange;
	ofNotifyEvent(ofxTLEvents.zoomStarted, zoomEvent);		
}

void ofxTLZoomer::notifyZoomDragged(ofRange oldRange){
	ofxTLZoomEventArgs zoomEvent;
	zoomEvent.oldZoom = oldRange;
	zoomEvent.currentZoom = currentViewRange;
	ofNotifyEvent(ofxTLEvents.zoomDragged, zoomEvent);
}

void ofxTLZoomer::notifyZoomEnded(){
	ofxTLZoomEventArgs zoomEvent;
	zoomEvent.currentZoom = zoomEvent.oldZoom = currentViewRange;
	ofNotifyEvent(ofxTLEvents.zoomEnded, zoomEvent);	
}

void ofxTLZoomer::keyPressed(ofKeyEventArgs& args){
	//TODO: nudge
}

ofRange ofxTLZoomer::getViewRange() {
	return currentViewRange;
}
