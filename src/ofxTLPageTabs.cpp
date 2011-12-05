/*
 *  ofxTLPageTabs.cpp
 *  timelineExample
 *
 *  Created by Jim on 10/2/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLPageTabs.h"

void ofxTLPageTabs::setup(){
	selectedPageIndex = -1;
}

void ofxTLPageTabs::draw(){
	ofPushStyle();
	for(int i = 0; i < pages.size(); i++){
		if(i == selectedPageIndex){
			ofFill();
			ofSetColor(255/4., 100/4., 0);
			ofRect(pages[i].bounds);
		}
		ofNoFill();
		ofSetColor(255, 100, 0);		
		ofRect(pages[i].bounds);
		
		ofDrawBitmapString(pages[i].name, pages[i].bounds.x + 10, pages[i].bounds.y + 10 );
	}
	ofPopStyle();
}

	
void ofxTLPageTabs::mouseReleased(ofMouseEventArgs& args){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i].bounds.inside(args.x, args.y)){
			selectPage(i);
		}
	}
}

void ofxTLPageTabs::addPage(string name){
	
	Tab newPage;
	newPage.name = name;
	pages.push_back(newPage);
	if(pages.size() == 1){
		selectPage(0);
	}
	drawRectChanged();
}

void ofxTLPageTabs::changeName(string oldName, string newName){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i].name == oldName){
			pages[i].name = newName;
		}	
	}
}

void ofxTLPageTabs::selectPage(int index){
	if(index >= pages.size()){
		ofLogError("ofxTLPageTabs -- Selecting page " + ofToString(index) + " out of range");
		return;
	}
	
	ofxTLPageEventArgs pageEvent;
	if(selectedPageIndex != -1){
		pageEvent.oldPageName = pages[selectedPageIndex].name;
	}
	pageEvent.currentPageName = pages[index].name;
	selectedPageIndex = index;
	
	ofNotifyEvent(ofxTLEvents.pageChanged, pageEvent);
}

void ofxTLPageTabs::selectPage(string name){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i].name == name){
			selectPage(i);
		}
	}
	ofLogError("ofxTLPageTabs -- Selecting page " + name + " not found");

}

void ofxTLPageTabs::keyPressed(ofKeyEventArgs& args){

}

void ofxTLPageTabs::drawRectChanged(){
	float rectWidth = bounds.width/pages.size();
	for(int i = 0; i < pages.size(); i++){
		pages[i].bounds = ofRectangle(bounds.x + rectWidth*i, bounds.y, rectWidth, bounds.height);
		cout << "# pages " << pages.size() << " Draw rect is " << pages[i].bounds.x << " " << pages[i].bounds.y << " " << pages[i].bounds.width << " " << pages[i].bounds.height << endl;
	}
}

