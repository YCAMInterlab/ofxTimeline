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
}

ofxTLElementHeader::~ofxTLElementHeader(){

}

void ofxTLElementHeader::setup(){

}

void ofxTLElementHeader::setElement(ofxTLElement* newElement){
	element = newElement;
}

void ofxTLElementHeader::draw(){
	ofPushStyle();
	
	ofSetColor(element->getTimeline()->getColors().outlineColor);
	
	ofNoFill();
	ofDrawBitmapString( name, ofPoint(bounds.x + 30, bounds.y + 15) );
	ofRect(bounds);
	
	//draw grippy lines on the footer draggable element
	ofSetColor(element->getTimeline()->getColors().outlineColor);
	float footerStartY = element->getDrawRect().y + element->getDrawRect().height;
	for(float l = bounds.x; l < bounds.x+bounds.width; l+=FOOTER_HEIGHT){
		ofLine(l+FOOTER_HEIGHT, footerStartY, l, footerStartY+FOOTER_HEIGHT);
	}
	
	ofPopStyle();
}

void ofxTLElementHeader::mousePressed(ofMouseEventArgs& args){
	ofRectangle elementRect = element->getDrawRect();
	float footerStartY = elementRect.y + elementRect.height;
		
	if (args.y > footerStartY && args.y < footerStartY + FOOTER_HEIGHT) {
		dragOffset = args.y - footerStartY;
		draggingSize = true;
	}
}

void ofxTLElementHeader::mouseMoved(ofMouseEventArgs& args){
	
}

void ofxTLElementHeader::mouseDragged(ofMouseEventArgs& args){
	if(draggingSize){
		ofRectangle elementRect = element->getDrawRect();
		float oldHeight = elementRect.height;
		elementRect.height = MAX(0, args.y - elementRect.y - dragOffset);
		element->setDrawRect(elementRect);
		
		ofEventArgs a;
		ofNotifyEvent(ofxTLEvents.viewNeedsResize, a);
	}
}

void ofxTLElementHeader::mouseReleased(ofMouseEventArgs& args){
	draggingSize = false;
}
