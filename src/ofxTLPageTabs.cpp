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

#include "ofxTLPageTabs.h"
#include "ofxTimeline.h"

void ofxTLPageTabs::setup(){
	selectedPageIndex = -1;
	pressedPageIndex = -1;
}

void ofxTLPageTabs::draw(){
	ofPushStyle();
	for(int i = 0; i < pages.size(); i++){
		if(i == selectedPageIndex){
			ofFill();
			ofSetColor(timeline->getColors().highlightColor, 120);
			ofDrawRectangle(pages[i].bounds);
		}
		ofNoFill();
		//ofSetColor(255, 100, 0);
		ofSetColor(timeline->getColors().outlineColor);
		ofDrawRectangle(pages[i].bounds);
		ofSetColor(timeline->getColors().textColor);
		timeline->getFont().drawString(pages[i].name, pages[i].bounds.x + 10, pages[i].bounds.y + timeline->getFont().getLineHeight());
	}
	ofPopStyle();
}

void ofxTLPageTabs::mousePressed(ofMouseEventArgs& args){
	pressedPageIndex = -1;
	for(int i = 0; i < pages.size(); i++){
		if(pages[i].bounds.inside(args.x, args.y)){
			pressedPageIndex = i;
			break;
		}
	}
}

void ofxTLPageTabs::mouseReleased(ofMouseEventArgs& args){
	if(pressedPageIndex != -1 && pages[pressedPageIndex].bounds.inside(args.x, args.y)){
		selectPage(pressedPageIndex);
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

void ofxTLPageTabs::removePage(string name){
	int found = -1;
	for (int i = 0; i < pages.size(); i++) {
		found = i;
		break;
	}

	if (found >= 0) {
		pages.erase(pages.begin() + found);
		drawRectChanged();
	}
}

void ofxTLPageTabs::changeName(string oldName, string newName){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i].name == oldName){
			pages[i].name = newName;
		}	
	}
}

void ofxTLPageTabs::selectPage(int index){
	if(index == selectedPageIndex){
		return;
	}
	
	if(index >= pages.size()){
		ofLogError("ofxTLPageTabs -- Selecting page " + ofToString(index) + " out of range");
		return;
	}
	
	ofxTLPageEventArgs pageEvent;
    pageEvent.sender = timeline;
	if(selectedPageIndex != -1){
		pageEvent.oldPageName = pages[selectedPageIndex].name;
	}
	pageEvent.currentPageName = pages[index].name;
	selectedPageIndex = index;
	
	ofNotifyEvent(events().pageChanged, pageEvent);
}

void ofxTLPageTabs::selectPage(string name){
	if(name == pages[selectedPageIndex].name){
		return;
	}
	for(int i = 0; i < pages.size(); i++){
		if(pages[i].name == name){
			selectPage(i);
            return;
		}
	}
	ofLogError("ofxTLPageTabs -- Selecting page " + name + " not found");
}

void ofxTLPageTabs::clear(){
	selectedPageIndex = -1;
	pages.clear();
}

void ofxTLPageTabs::keyPressed(ofKeyEventArgs& args){

}

void ofxTLPageTabs::drawRectChanged(){
	float rectWidth = bounds.width/pages.size();
	for(int i = 0; i < pages.size(); i++){
		pages[i].bounds = ofRectangle(bounds.x + rectWidth*i, bounds.y, rectWidth, bounds.height);
	}
}

int ofxTLPageTabs::getSelectedPageIndex() {
    return selectedPageIndex;
}

string ofxTLPageTabs::getSelectedPageName() {
    return pages.size() > 0 ? pages[selectedPageIndex].name : "";
}
