/*
 *  ofxTLElementHeader.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
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
//	ofRectangle elementRect = element->getDrawRect();
//	float footerStartY = elementRect.y + elementRect.height;
		
	//if (args.y > footerStartY && args.y < footerStartY + FOOTER_HEIGHT) {
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
		float oldHeight = elementRect.height;
		elementRect.height = MAX(0, args.y - elementRect.y - dragOffset);
		element->setDrawRect(elementRect);
		
		float footerStartY = elementRect.y + elementRect.height;
		footerRect = ofRectangle(bounds.x, footerStartY, bounds.width, FOOTER_HEIGHT);
		
		ofEventArgs a;
		ofNotifyEvent(ofxTLEvents.viewNeedsResize, a);
	}
}

void ofxTLElementHeader::mouseReleased(ofMouseEventArgs& args){
	draggingSize = false;
}
