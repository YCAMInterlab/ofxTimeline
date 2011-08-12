/*
 *  ofxTLElementHeader.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLElementHeader.h"

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
	
	ofSetColor(250, 180, 80);
	
	ofNoFill();
	ofDrawBitmapString( name + " header la la la ", ofPoint(bounds.x + 30, bounds.y + 10) );
	ofRect(bounds);
	
	ofPopStyle();
}

void ofxTLElementHeader::mousePressed(ofMouseEventArgs& args){
	ofRectangle elementRect = element->getDrawRect();
	float footerStartY = elementRect.y + elementRect.height;
	
	cout << "args touched at " << args.y << " footer start is " << footerStartY << endl;
	
	if (args.y > footerStartY && args.y < footerStartY + FOOTER_HEIGHT) {
		dragOffset = args.y - footerStartY;
		draggingSize = true;
		cout << "dragging started" << endl;
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
		cout << "notifying event. old height " << oldHeight << " new height " << elementRect.height << endl;

	}
}

void ofxTLElementHeader::mouseReleased(ofMouseEventArgs& args){
	draggingSize = false;
}
