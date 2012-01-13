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

#include "ofxTLElementHeader.h"
#include "ofxTimeline.h"

ofxTLElementHeader::ofxTLElementHeader(){
	element = NULL;
	draggingSize = false;
	hoveringFooter = false;
}

ofxTLElementHeader::~ofxTLElementHeader(){

}

void ofxTLElementHeader::setup(){
	
}

void ofxTLElementHeader::setElement(ofxTLElement* newElement){
	element = newElement;

}

void ofxTLElementHeader::draw(){
	ofRectangle elementRect = element->getDrawRect();
	float footerStartY = elementRect.y + elementRect.height;
	footerRect = ofRectangle(bounds.x, footerStartY, bounds.width, FOOTER_HEIGHT);	
	
	ofPushStyle();
	
	ofNoFill();
	ofSetColor(element->getTimeline()->getColors().textColor);
	ofDrawBitmapString( name, ofPoint(bounds.x + 30, bounds.y + 15) );
	
	ofSetColor(element->getTimeline()->getColors().outlineColor);
	ofRect(bounds);
	
	//draw grippy lines on the footer draggable element
	if(draggingSize){
		ofSetColor(element->getTimeline()->getColors().highlightColor);
	}
	else if(hoveringFooter){
		ofSetColor(element->getTimeline()->getColors().outlineColor);
	}
	else{
		ofSetColor(element->getTimeline()->getColors().disabledColor);
	}
	for(float l = bounds.x; l < bounds.x+bounds.width; l+=FOOTER_HEIGHT){
		ofLine(l+FOOTER_HEIGHT, footerStartY, l, footerStartY+footerRect.height);
	}
	
	ofPopStyle();
}

void ofxTLElementHeader::mousePressed(ofMouseEventArgs& args){
	if(footerRect.inside(ofPoint(args.x,args.y))){
		dragOffset = args.y - footerRect.y;
		draggingSize = true;
	}
}

void ofxTLElementHeader::mouseMoved(ofMouseEventArgs& args){
	hoveringFooter = footerRect.inside(args.x, args.y);
}

void ofxTLElementHeader::mouseDragged(ofMouseEventArgs& args){
	if(draggingSize){
		ofRectangle elementRect = element->getDrawRect();
		elementRect.height = MAX(0, args.y - elementRect.y - dragOffset);
		element->setDrawRect(elementRect);
		
		recalculateFooter();		
	}
}

void ofxTLElementHeader::collapseElement(){
	ofRectangle elementRect = element->getDrawRect();
	elementRect.height = 0;
	element->setDrawRect(elementRect);	
	recalculateFooter();
}

void ofxTLElementHeader::recalculateFooter(){
	ofRectangle elementRect = element->getDrawRect();
	float footerStartY = elementRect.y + elementRect.height;
	footerRect = ofRectangle(bounds.x, footerStartY, bounds.width, FOOTER_HEIGHT);
	
	ofEventArgs a;
	ofNotifyEvent(ofxTLEvents.viewWasResized, a);
}

void ofxTLElementHeader::mouseReleased(ofMouseEventArgs& args){
	draggingSize = false;
}

ofRectangle ofxTLElementHeader::getFooterRect(){
	return footerRect;
}