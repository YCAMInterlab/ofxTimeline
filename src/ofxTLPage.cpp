/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
 * implementaiton by James George (@obviousjim) and Tim Gfrerer (@tgfrerer) for the 
 * Voyagers gallery National Maritime Museum 
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

#include "ofxTLPage.h"
#include "ofxTLTicker.h"

ofxTLPage::ofxTLPage()
:	autosave(false),
	elementContainerRect(ofRectangle(0,0,0,0)),
	headerHeight(0),
	defaultElementHeight(0),
	isSetup(false),
	snappingTolerance(12),
	ticker(NULL),
	snapToOtherElementsEnabled(true),
	headerHasFocus(false)
{
	//
}

ofxTLPage::~ofxTLPage(){
	if(isSetup){
		for(int i = 0; i < headers.size(); i++){
			if(elements[headers[i]->name]->getCreatedByTimeline()){
				delete elements[headers[i]->name];
			}
			delete headers[i];
		}
		headers.clear();
		elements.clear();
		
		ofRemoveListener(ofxTLEvents.zoomEnded, this, &ofxTLPage::zoomEnded);
	}
}

#pragma mark utility
void ofxTLPage::setup(){
	
	ofAddListener(ofxTLEvents.zoomEnded, this, &ofxTLPage::zoomEnded);
	isSetup = true;
	headerHeight = 20;
	defaultElementHeight = 50;
	loadElementPositions(); //name must be set
}

//given a folder the page will look for xml files to load within that
void ofxTLPage::loadElementsFromFolder(string folderPath){
    for(int i = 0; i < headers.size(); i++){
		string filename = folderPath + elements[headers[i]->name]->getXMLFileName();
        elements[headers[i]->name]->setXMLFileName(filename);
        elements[headers[i]->name]->load();
    }
}

void ofxTLPage::setTicker(ofxTLTicker* t){
	ticker = t;
}

void ofxTLPage::draw(){	
	for(int i = 0; i < headers.size(); i++){
		headers[i]->draw();
		elements[headers[i]->name]->draw();
	}	
	if(draggingInside && snappingEnabled){
		ofPushStyle();
		ofSetColor(255,255,255,100);
		for(int i = 0; i < snapPoints.size(); i++){
			ofLine(snapPoints[i], elementContainerRect.y, snapPoints[i], elementContainerRect.y+elementContainerRect.height);
		}
		ofPopStyle();
	}
	
	for(int i = 0; i < headers.size(); i++){
		elements[headers[i]->name]->drawModalContent();
	}	
	
}

#pragma mark events
void ofxTLPage::mousePressed(ofMouseEventArgs& args){
	draggingInside = elementContainerRect.inside(args.x, args.y);
	if(draggingInside){
		if(snappingEnabled){
			refreshSnapPoints();
		}
		headerHasFocus = false;
		for(int i = 0; i < headers.size(); i++){
			headers[i]->mousePressed(args);
			elements[headers[i]->name]->mousePressed(args);
			headerHasFocus |= ( headers[i]->getFooterRect().inside(args.x,args.y) && !elements[headers[i]->name]->getDrawRect().inside(args.x,args.y) );
		}
	}
}

void ofxTLPage::mouseMoved(ofMouseEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseMoved(args);
		elements[headers[i]->name]->mouseMoved(args);
	}	
}

void ofxTLPage::mouseDragged(ofMouseEventArgs& args){
	if(draggingInside){
		bool snapped = false;
		float snapPercent = 0;
		if(snappingEnabled){
			
			refreshSnapPoints();
			
			float closestSnapDistance = snappingTolerance;
			float closestSnapPoint;
			//modify drag X if it should be snapped
			for(int i = 0; i < snapPoints.size(); i++){
				float distanceToPoint = abs(args.x - snapPoints[i]);
				if(distanceToPoint < closestSnapDistance){
					closestSnapPoint = i;
					closestSnapDistance = distanceToPoint; 
				}
			}
			
			if(closestSnapDistance < snappingTolerance){
				args.x = snapPoints[closestSnapPoint] + dragAnchor;
				snapPercent = snapPoints[closestSnapPoint];
				snapped = true;
			}
		}
		
		for(int i = 0; i < headers.size(); i++){
			headers[i]->mouseDragged(args);
			if(!headerHasFocus){
				elements[headers[i]->name]->mouseDragged(args, snapped);
			}
		}
	}
}

void ofxTLPage::setDragAnchor(float anchor){
	dragAnchor = anchor;
}
void ofxTLPage::mouseReleased(ofMouseEventArgs& args){
	if(draggingInside){
		for(int i = 0; i < headers.size(); i++){
			headers[i]->mouseReleased(args);
			elements[headers[i]->name]->mouseReleased(args);
		}		
		draggingInside = false;
	}
}

void ofxTLPage::refreshSnapPoints(){
	//get the snapping points
	snapPoints.clear();
	if(snapToOtherElementsEnabled){
		for(int i = 0; i < headers.size(); i++){
			elements[headers[i]->name]->getSnappingPoints(snapPoints);	
		}
	}
	if(ticker != NULL){
		ticker->getSnappingPoints(snapPoints);
	}	
}

//copy paste
string ofxTLPage::copyRequest(){
	string buf;
	for(int i = 0; i < headers.size(); i++){
		buf += elements[headers[i]->name]->copyRequest();
	}	
	return buf;	
}

string ofxTLPage::cutRequest(){
	string buf;
	for(int i = 0; i < headers.size(); i++){
		buf += elements[headers[i]->name]->cutRequest();
	}	
	return buf;
}

void ofxTLPage::pasteSent(string pasteboard){
	for(int i = 0; i < headers.size(); i++){
		//only paste into where we are hovering
		if(elements[headers[i]->name]->getDrawRect().inside( ofVec2f(ofGetMouseX(), ofGetMouseY()) )){ //TODO: replace with hasFocus()
			elements[headers[i]->name]->pasteSent(pasteboard);
		}
	}	
}
		
void ofxTLPage::selectAll(){
	for(int i = 0; i < headers.size(); i++){
		//only paste into where we are hovering
		if(elements[headers[i]->name]->getDrawRect().inside( ofVec2f(ofGetMouseX(), ofGetMouseY()) )){ //TODO: replace with hasFocus()
			elements[headers[i]->name]->selectAll();
		}
	}
}

void ofxTLPage::keyPressed(ofKeyEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		elements[headers[i]->name]->keyPressed(args);
	}
}

void ofxTLPage::nudgeBy(ofVec2f nudgePercent){
	for(int i = 0; i < headers.size(); i++){
		elements[headers[i]->name]->nudgeBy(nudgePercent);
	}
}

void ofxTLPage::addElement(string elementName, ofxTLElement* element){

	ofxTLElementHeader* newHeader = new ofxTLElementHeader();
	newHeader->setElement(element);
	newHeader->setup();

	//	cout << "adding " << name << " current zoomer is " << zoomer->getDrawRect().y << endl;

	ofRectangle newHeaderRect = ofRectangle(elementContainerRect.x, elementContainerRect.height, elementContainerRect.width, headerHeight);
	newHeader->setDrawRect(newHeaderRect);
	newHeader->name = elementName;

	headers.push_back(newHeader);

	element->setup();
	element->setAutosave(autosave);
	
	ofRectangle drawRect;
	if(savedElementPositions.find(elementName) != savedElementPositions.end()){
		drawRect = savedElementPositions[elementName];
	}
	else {
		drawRect = ofRectangle(0, newHeaderRect.y+newHeaderRect.height, ofGetWidth(), defaultElementHeight);
	}

	element->setDrawRect(drawRect);
	element->setZoomBounds(currentZoomBounds);

	elements[elementName] = element;
}

ofxTLElement* ofxTLPage::getElement(string elementName){
	if(elements.find(elementName) == elements.end()){
		ofLogError("ofxTLPage -- Couldn't find element named " + elementName + " on page " + name);
		return NULL;
	}
	return elements[elementName];
}

void ofxTLPage::collapseAllElements(){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->collapseElement();
	}
	recalculateHeight();
}

void ofxTLPage::removeElement(string name){
	//TODO:
}

void ofxTLPage::recalculateHeight(){
	float currentY = elementContainerRect.y;
	//float currentY = containerOffset.y;
	float totalHeight = 0;
	for(int i = 0; i < headers.size(); i++){
		ofRectangle thisHeader = headers[i]->getDrawRect();
		ofRectangle elementRectangle = elements[ headers[i]->name ]->getDrawRect();
		
		float startY = thisHeader.y+thisHeader.height;
		float endY = startY + elementRectangle.height;
		
		thisHeader.width = elementContainerRect.width;
		thisHeader.y = currentY;
		headers[i]->setDrawRect(thisHeader);
		elementRectangle.y = startY;
		elementRectangle.width = elementContainerRect.width;
		
		elements[ headers[i]->name ]->setDrawRect( elementRectangle );
		currentY += thisHeader.height + elementRectangle.height + FOOTER_HEIGHT;
		totalHeight += thisHeader.height + elementRectangle.height + FOOTER_HEIGHT;
		
		savedElementPositions[headers[i]->name] = elementRectangle;

	}
	
	//computedHeight = totalHeight;
	elementContainerRect.height = totalHeight;
	if(autosave){
		saveElementPositions();
	}
}

float ofxTLPage::getComputedHeight(){
	//return computedHeight;
	return elementContainerRect.height;
}

void ofxTLPage::setSnapping(bool snapping){
	snappingEnabled = snapping;
}

void ofxTLPage::enableSnapToOtherElements(bool snapToOthes){
	snapToOtherElementsEnabled = snapToOthes;
}

#pragma mark saving/restoring state
void ofxTLPage::loadElementPositions(){
	ofxXmlSettings elementPositions;
	if(elementPositions.loadFile(name + "_elementPositions.xml")){
		
		//cout << "loading element position " << name << "_elementPositions.xml" << endl;
		
		elementPositions.pushTag("positions");
		int numElements = elementPositions.getNumTags("element");
		for(int i = 0; i < numElements; i++){
			string name = elementPositions.getAttribute("element", "name", "", i);
			elementPositions.pushTag("element", i);
			ofRectangle elementPosition = ofRectangle(ofToFloat(elementPositions.getValue("x", "0")),
													  ofToFloat(elementPositions.getValue("y", "0")),
													  ofToFloat(elementPositions.getValue("width", "0")),
													  ofToFloat(elementPositions.getValue("height", "0")));
			savedElementPositions[name] = elementPosition;
			elementPositions.popTag();
		}
		elementPositions.popTag();
	}
}

void ofxTLPage::saveElementPositions(){
	ofxXmlSettings elementPositions;
	elementPositions.addTag("positions");
	elementPositions.pushTag("positions");
	
	int curElement = 0;
	map<string, ofRectangle>::iterator it;
	for(it = savedElementPositions.begin(); it != savedElementPositions.end(); it++){
		elementPositions.addTag("element");
		elementPositions.addAttribute("element", "name", it->first, curElement);
		
		elementPositions.pushTag("element", curElement);
		elementPositions.addValue("x", it->second.x);
		elementPositions.addValue("y", it->second.y);
		elementPositions.addValue("width", it->second.width);
		elementPositions.addValue("height", it->second.height);
		
		elementPositions.popTag(); //element
		
		curElement++;
	}
	elementPositions.popTag();
	elementPositions.saveFile(name + "_elementPositions.xml");
}


#pragma mark getters/setters
void ofxTLPage::setZoomBounds(ofRange zoomBounds){
	currentZoomBounds = zoomBounds;
}

void ofxTLPage::zoomEnded(ofxTLZoomEventArgs& args){
	currentZoomBounds = args.currentZoom;
}

void ofxTLPage::setName(string newName){
	name = newName;
}

void ofxTLPage::unselectAll(){
	for(int i = 0; i < headers.size(); i++){
		elements[headers[i]->name]->unselectAll();
	}	
}

void ofxTLPage::reset(){
	for(int i = 0; i < headers.size(); i++){
		//only paste into where we are hovering
		elements[headers[i]->name]->reset();
    }
}

void ofxTLPage::save(){
	for(int i = 0; i < headers.size(); i++){
		//only paste into where we are hovering
		elements[headers[i]->name]->save();
    }
}

string ofxTLPage::getName(){
	return name;
}

void ofxTLPage::setAutosave(bool doAutosave){
	autosave = doAutosave;
}

void ofxTLPage::setContainer(ofVec2f offset, float width){
	elementContainerRect.x = offset.x;
	elementContainerRect.y = offset.y;
	elementContainerRect.width = width;
	recalculateHeight();
}

void ofxTLPage::setHeaderHeight(float newHeaderHeight){
	headerHeight = newHeaderHeight;
	recalculateHeight();
}

void ofxTLPage::setDefaultElementHeight(float newDefaultElementHeight){
	defaultElementHeight = newDefaultElementHeight;
}

