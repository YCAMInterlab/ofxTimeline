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
            return;
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
	}
}

