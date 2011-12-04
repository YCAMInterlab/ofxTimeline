/*
 *  ofxTLTrigger.cpp
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLTrigger.h"
#include "ofxTimeline.h";

bool triggersort(Trigger a, Trigger b){
	return a.pt < b.pt;
}

ofxTLTrigger::ofxTLTrigger(){
	enterText = false;
	selectedTrigger = hoverTrigger = NULL;
}

ofxTLTrigger::~ofxTLTrigger(){
	
}

void ofxTLTrigger::setup(){
	
}

void ofxTLTrigger::draw(){
	ofPushStyle();
	//**** DRAW BORDER
	ofNoFill();
	
	if(hover){
		//ofSetColor(255, 0, 0);
		ofSetColor(timeline->getColors().highlightColor);
	}
	else if(focused){
		//ofSetColor(255, 200, 0); //focused outline color
		ofSetColor(timeline->getColors().highlightColor);
	}
	else{
		//ofSetColor(150, 150, 0); //unfocused outline color
		ofSetColor(timeline->getColors().outlineColor);
	}
	
	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	
	ofSetLineWidth(5);
	for(int i = triggers.size()-1; i >= 0; i--){
		int screenX = normalizedXtoScreenX(triggers[i].pt);
		if(&triggers[i] == selectedTrigger){
			ofSetColor(timeline->getColors().textColor);
		}
		else if(&triggers[i] == hoverTrigger){
			ofSetColor(timeline->getColors().highlightColor);
		}
		else{
			ofSetColor(timeline->getColors().keyColor);
		}
		
		ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
		ofSetColor(timeline->getColors().textColor);
		int textHeight = bounds.y + 10 + ( (20*i) % int(bounds.height) );
		
		string timeString = timeline->getIsFrameBased() ? 
			ofToString(indexForScreenX(screenX)) : 
			ofToString(triggers[i].pt * timeline->getDurationInSeconds());
		
		ofDrawBitmapString(timeString+" | " +triggers[i].name, screenX+5, textHeight);
	}
	ofPopStyle();
}

//main function to get values out of the timeline, operates on the given value range
void ofxTLTrigger::mousePressed(ofMouseEventArgs& args){
	ofxTLElement::mousePressed(args);
	
	enterText = false;
	selectedTrigger = getTriggerForScreenPosition(args.x, dragOffset);
	if(selectedTrigger == NULL){
		Trigger t;
		t.pt = screenXtoNormalizedX(args.x);
		t.name = "new trigger";
		triggers.push_back(t);
		sort(triggers.begin(), triggers.end(), triggersort);
	}
	
}

void ofxTLTrigger::mouseMoved(ofMouseEventArgs& args){
	ofxTLElement::mouseMoved(args);	
	int unused;
	hoverTrigger = getTriggerForScreenPosition(args.x, unused);
}

void ofxTLTrigger::mouseDragged(ofMouseEventArgs& args){
	ofxTLElement::mouseDragged(args);
	if(selectedTrigger != NULL){
		selectedTrigger->pt = screenXtoNormalizedX(args.x - dragOffset);
	}
}

void ofxTLTrigger::mouseReleased(ofMouseEventArgs& args){
	ofxTLElement::mouseReleased(args);	
	if(selectedTrigger != NULL){
		sort(triggers.begin(), triggers.end(), triggersort);
	}
}

void ofxTLTrigger::keyPressed(ofKeyEventArgs& args){
	if(args.key == OF_KEY_RETURN && selectedTrigger != NULL){
		enterText = !enterText;
	}
}

void ofxTLTrigger::save(){
	
}

void ofxTLTrigger::load(){
	
}

void ofxTLTrigger::clear(){
	triggers.clear();
}

Trigger* ofxTLTrigger::getTriggerForScreenPosition(float screenx, int& offset){
	for(int i = 0; i < triggers.size(); i++){
		offset = screenx - normalizedXtoScreenX(triggers[i].pt);
		if (abs(offset) < 10) {
			return &triggers[i];
		}
	}
	return NULL;
}
