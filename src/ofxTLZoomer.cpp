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

#include "ofxTLZoomer.h"
#include "ofxXmlSettings.h"
#include "ofRange.h"
#include "ofxTimeline.h"

ofxTLZoomer::ofxTLZoomer()
:	minSelected(false),
	maxSelected(false),
	midSelected(false),
	currentViewRange(ofRange(0.0, 1.0)),
	zoomExponent(2.0)
{
	//default constructor
}

ofxTLZoomer::~ofxTLZoomer(){
	
}

void ofxTLZoomer::draw(){
	
	ofPushStyle();
	ofSetColor(timeline->getColors().textColor);
	//draw min
	float screenY = bounds.y + bounds.height/2.0;
	float minScreenX = normalizedXtoScreenX(currentViewRange.min, ofRange(0,1.0));
	float maxScreenX = normalizedXtoScreenX(currentViewRange.max, ofRange(0,1.0));

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

//	cout << "zoomer bounds width " << bounds.width << endl;
	//draw playhead reference
	ofLine(bounds.x+bounds.width*timeline->getPercentComplete(), bounds.y,
		   bounds.x+bounds.width*timeline->getPercentComplete(), bounds.y+bounds.height);
	//draw zoom region reference
	ofSetColor(timeline->getColors().backgroundColor);
	ofRange actualZoom = getViewRange();
	ofRectangle zoomRegion = ofRectangle(bounds.x + bounds.width*actualZoom.min, bounds.y,
										 bounds.width*actualZoom.span(),bounds.height);
	ofFill();
	ofSetColor(timeline->getColors().keyColor, 50);
	ofRect(zoomRegion);
	ofPopStyle();
}

void ofxTLZoomer::load() {

	notifyZoomStarted();
	
	ofxXmlSettings settings;
	if(!settings.loadFile(xmlFileName)){
		ofLog(OF_LOG_VERBOSE, "ofxTLZoomer -- couldn't load zoom settings file " + xmlFileName);
        currentViewRange = ofRange(0., 1.0);
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
	
	minSelected = maxSelected = midSelected = false;
	if (pointInScreenBounds(ofVec2f(args.x, args.y))) {
		mouseIsDown = true;
		
		//did we click on the min-left handle?
		float minScreenX = normalizedXtoScreenX(currentViewRange.min, ofRange(0,1.0));
		minGrabOffset = args.x - minScreenX;
		if(fabs(minScreenX - args.x) < 5){
			minSelected = true;
			notifyZoomStarted();
			return;
		}
		
		//did we click on the max-right handle?
		float maxScreenX = normalizedXtoScreenX(currentViewRange.max, ofRange(0,1.0));
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
			currentViewRange.max = screenXtoNormalizedX(args.x, ofRange(0,1.0));
			notifyZoomStarted();
			return;
		}
		
		//did we click to the left?
		if(args.x < minScreenX){
			minSelected = true;
			minGrabOffset = 0;
			currentViewRange.min = screenXtoNormalizedX(args.x, ofRange(0,1.0));
			notifyZoomStarted();
			return;
		}
	}
}

void ofxTLZoomer::mouseDragged(ofMouseEventArgs& args) {
    
	if(!enabled) return;
	
    bool notify = false;
	ofRange oldRange = getViewRange();
	if(minSelected || midSelected){
		float originalMin = currentViewRange.min;
		currentViewRange.min = ofClamp( screenXtoNormalizedX(args.x-minGrabOffset, ofRange(0, 1.0)), 0, currentViewRange.max-.01);
		if(minSelected){
			currentViewRange.max = ofClamp( currentViewRange.max + (originalMin-currentViewRange.min), currentViewRange.min+.01, 1.0);
		}
		notify = true;
	}

	if(maxSelected || midSelected){
		float originalMax = currentViewRange.max;
		currentViewRange.max = ofClamp( screenXtoNormalizedX(args.x-maxGrabOffset, ofRange(0, 1.0)), currentViewRange.min+.01, 1.0);
		if(maxSelected){
			currentViewRange.min = ofClamp( currentViewRange.min + (originalMax-currentViewRange.max), 0, currentViewRange.max-.01);
		}
        notify = true;
    }
	
    if(notify){
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
//		timeline->flagTrackModified(this);
		save(); //intentionally ignores auto save since this is just a view parameter
	}
}

void ofxTLZoomer::lostFocus(){
	ofxTLTrack::lostFocus();
	minSelected = false;
	maxSelected = false;
	midSelected = false;
}

void ofxTLZoomer::notifyZoomStarted(){
	ofxTLZoomEventArgs zoomEvent;
    zoomEvent.sender = timeline;
	zoomEvent.currentZoom = zoomEvent.oldZoom = getViewRange();
	ofNotifyEvent(events().zoomStarted, zoomEvent);		
}

void ofxTLZoomer::notifyZoomDragged(ofRange oldRange){
	ofxTLZoomEventArgs zoomEvent;
    zoomEvent.sender = timeline;
	zoomEvent.oldZoom = oldRange;
	//zoomEvent.currentZoom = currentViewRange;
	zoomEvent.currentZoom = getViewRange();
	ofNotifyEvent(events().zoomDragged, zoomEvent);
}

void ofxTLZoomer::notifyZoomEnded(){
	ofxTLZoomEventArgs zoomEvent;
    zoomEvent.sender = timeline;    
	//zoomEvent.currentZoom = currentViewRange;
	zoomEvent.currentZoom = getViewRange();
	ofNotifyEvent(events().zoomEnded, zoomEvent);
}

void ofxTLZoomer::keyPressed(ofKeyEventArgs& args){
	//TODO: Nudging?
}

ofRange ofxTLZoomer::getViewRange() {
	float logSpan = powf(currentViewRange.span(),zoomExponent);
	float centerPosition = currentViewRange.center();
	//recompute view range
	if(centerPosition != .5){
		centerPosition = ofMap(centerPosition, currentViewRange.span()/2, 1.0 - currentViewRange.span()/2, logSpan/2, 1.0-logSpan/2);
	}
	return ofRange(centerPosition - logSpan/2, centerPosition + logSpan/2);
}

void ofxTLZoomer::setViewExponent(float exponent){
	zoomExponent = 1.0;
	setViewRange(currentViewRange);
}

void ofxTLZoomer::setViewRange(ofRange newRange){

	ofxTLZoomEventArgs zoomEvent;
    zoomEvent.oldZoom = getViewRange();
    zoomEvent.sender = timeline;

	currentViewRange = newRange;
	zoomEvent.currentZoom = getViewRange();
	ofNotifyEvent(events().zoomEnded, zoomEvent); 
}

ofRange ofxTLZoomer::getSelectedRange(){
	return currentViewRange;
}

