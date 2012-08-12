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

#include "ofxTLTrackHeader.h"
#include "ofxTimeline.h"

ofxTLTrackHeader::ofxTLTrackHeader(){
	track = NULL;
	draggingSize = false;
	hoveringFooter = false;
}

ofxTLTrackHeader::~ofxTLTrackHeader(){

}

void ofxTLTrackHeader::enable(){
	nameField.setup();    
    ofAddListener(nameField.textChanged, this, &ofxTLTrackHeader::textFieldEnter);

}

void ofxTLTrackHeader::disable(){
    ofRemoveListener(nameField.textChanged, this, &ofxTLTrackHeader::textFieldEnter);
}

void ofxTLTrackHeader::setTrack(ofxTLTrack* newTrack){
	track = newTrack;
}

ofxTLTrack* ofxTLTrackHeader::getTrack(){
	return track;    
}

void ofxTLTrackHeader::textFieldEnter(string& newText){
    cout << "text field entered to " << newText << endl;
    if(newText == ""){
        nameField.text = name;
    }
    else{
        if(track != NULL){
            track->setDisplayName(newText);
        }
    }
	track->getTimeline()->dismissedModalContent();
}

void ofxTLTrackHeader::draw(){
	ofRectangle trackRect = track->getDrawRect();
	float footerStartY = trackRect.y + trackRect.height;
	footerRect = ofRectangle(bounds.x, footerStartY, bounds.width, FOOTER_HEIGHT);	
	
	ofPushStyle();
	
	ofNoFill();
	ofSetColor(getTimeline()->getColors().textColor);
	// TODO: set these somewhere else instead of setting it every frame here
    // set name if it's empty and we're not editing
    if(nameField.text != track->getDisplayName() && !nameField.getIsEnabled()){
    	nameField.text = track->getDisplayName();   
    }
    
    if(nameField.getIsEnabled()){
    	track->getTimeline()->presentedModalContent(this);
    }
    
    nameField.bounds.x = bounds.x;
    nameField.bounds.y = bounds.y;
    nameField.draw();
	
	ofSetColor(track->getTimeline()->getColors().outlineColor);
	ofRect(bounds);
	
	//draw grippy lines on the footer draggable element
	if(draggingSize){
		ofSetColor(track->getTimeline()->getColors().highlightColor);
	}
	else if(hoveringFooter){
		ofSetColor(track->getTimeline()->getColors().outlineColor);
	}
	else{
		ofSetColor(track->getTimeline()->getColors().disabledColor);
	}
    
	for(float l = bounds.x; l < bounds.x+bounds.width; l+=FOOTER_HEIGHT){
		ofLine(l+FOOTER_HEIGHT, footerStartY, l, footerStartY+footerRect.height);
	}
	
	ofPopStyle();
}

//string ofxTLTrackHeader::getDisplayName() {
//    return nameField.text;
//}

void ofxTLTrackHeader::mousePressed(ofMouseEventArgs& args){
	if(footerRect.inside(ofPoint(args.x,args.y))){
		dragOffset = args.y - footerRect.y;
		draggingSize = true;
	}
}

void ofxTLTrackHeader::mouseMoved(ofMouseEventArgs& args){
	hoveringFooter = footerRect.inside(args.x, args.y);
}

void ofxTLTrackHeader::mouseDragged(ofMouseEventArgs& args){
	if(draggingSize){
		ofRectangle trackRect = track->getDrawRect();
		trackRect.height = MAX(0, args.y - trackRect.y - dragOffset);
		track->setDrawRect(trackRect);		
		recalculateFooter();		
	}
}

void ofxTLTrackHeader::collapseTrack(){
	ofRectangle trackRect = track->getDrawRect();
	trackRect.height = 0;
	track->setDrawRect(trackRect);	
	recalculateFooter();
}

void ofxTLTrackHeader::recalculateFooter(){
	ofRectangle trackRect = track->getDrawRect();
	float footerStartY = trackRect.y + trackRect.height;
	footerRect = ofRectangle(bounds.x, footerStartY, bounds.width, FOOTER_HEIGHT);
	
	ofEventArgs a;
	ofNotifyEvent(track->events().viewWasResized, a);
}

void ofxTLTrackHeader::mouseReleased(ofMouseEventArgs& args){
	draggingSize = false;
}

ofRectangle ofxTLTrackHeader::getFooterRect(){
	return footerRect;
}