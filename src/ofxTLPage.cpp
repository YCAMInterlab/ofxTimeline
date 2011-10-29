/*
 *  ofxTLPage.cpp
 *  timelineExample
 *
 *  Created by Jim on 10/2/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLPage.h"

//#define HEADER_HEIGHT 20 //TODO: make this variable
//#define DEFAULT_ELEMENT_HEIGHT 150 //TODO: make this variable

ofxTLPage::ofxTLPage()
:	autosave(false),
	computedHeight(0),
	containerWidth(0),
	headerHeight(0),
	defaultElementHeight(0),
	isSetup(false)
{
	//
}

ofxTLPage::~ofxTLPage(){
	if(isSetup){
		for(int i = 0; i < headers.size(); i++){
			delete elements[headers[i]->name];
			delete headers[i];
		}
		headers.clear();
		elements.clear();
		
		ofRemoveListener(ofxTLEvents.zoomEnded, this, &ofxTLPage::zoomEnded);
	}
}

#pragma mark utility
void ofxTLPage::setup(){
	
//	loadElementPositions();
	ofAddListener(ofxTLEvents.zoomEnded, this, &ofxTLPage::zoomEnded);
	isSetup = true;
	headerHeight = 20;
	defaultElementHeight = 200;
	loadElementPositions(); //name must be set
}

void ofxTLPage::draw(){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->draw();
		elements[headers[i]->name]->draw();
	}	
}

#pragma mark events
void ofxTLPage::mousePressed(ofMouseEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mousePressed(args);
		elements[headers[i]->name]->mousePressed(args);
	}	
}

void ofxTLPage::mouseMoved(ofMouseEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseMoved(args);
		elements[headers[i]->name]->mouseMoved(args);
	}
	
}

void ofxTLPage::mouseDragged(ofMouseEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseDragged(args);
		elements[headers[i]->name]->mouseDragged(args);
	}
}

void ofxTLPage::mouseReleased(ofMouseEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseReleased(args);
		elements[headers[i]->name]->mouseReleased(args);
	}		
}

void ofxTLPage::keyPressed(ofKeyEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		elements[headers[i]->name]->keyPressed(args);
	}
}

void ofxTLPage::addElement(string elementName, ofxTLElement* element){

	ofxTLElementHeader* newHeader = new ofxTLElementHeader();
	newHeader->setElement(element);
	newHeader->setup();

	//	cout << "adding " << name << " current zoomer is " << zoomer->getDrawRect().y << endl;

	ofRectangle newHeaderRect = ofRectangle(containerOffset.x, computedHeight, containerWidth, headerHeight);
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

	//	for(int i = 0; i < headers.size(); i++){
	//		cout << "	POST ADD header " << i << " is " << headers[i]->name << " y " << headers[i]->getDrawRect().y << " height " << headers[i]->getDrawRect().height << endl;
	//	}
}

void ofxTLPage::recalculateHeight(){
	
	float currentY = containerOffset.y;
	float totalHeight = 0;
	for(int i = 0; i < headers.size(); i++){
		ofRectangle thisHeader = headers[i]->getDrawRect();
		ofRectangle elementRectangle = elements[ headers[i]->name ]->getDrawRect();
		
		float startY = thisHeader.y+thisHeader.height;
		float endY = startY + elementRectangle.height;
		
		thisHeader.width = containerWidth;
		thisHeader.y = currentY;
		headers[i]->setDrawRect(thisHeader);
		elementRectangle.y = startY;
		elementRectangle.width = containerWidth;
		
		elements[ headers[i]->name ]->setDrawRect( elementRectangle );
		currentY += thisHeader.height + elementRectangle.height + FOOTER_HEIGHT;
		totalHeight += thisHeader.height + elementRectangle.height + FOOTER_HEIGHT;
		
		savedElementPositions[headers[i]->name] = elementRectangle;

	}
	
	computedHeight = totalHeight;
	
	if(autosave){
		saveElementPositions();
	}
}

float ofxTLPage::getComputedHeight(){
	return computedHeight;
}

#pragma mark saving/restoring state
void ofxTLPage::loadElementPositions(){
	ofxXmlSettings elementPositions;
	if(elementPositions.loadFile(name + "_elementPositions.xml")){
		
		cout << "loading element position " << name << "_elementPositions.xml" << endl;
		
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
void ofxTLPage::zoomEnded(ofxTLZoomEventArgs& args){
	currentZoomBounds = args.currentZoom;
}

void ofxTLPage::setName(string newName){
	name = newName;
}

string ofxTLPage::getName(){
	return name;
}

void ofxTLPage::setAutosave(bool doAutosave){
	autosave = doAutosave;
}

void ofxTLPage::setContainer(ofVec2f offset, float width){
	containerOffset = offset;
	containerWidth = width;
	recalculateHeight();
}

void ofxTLPage::setHeaderHeight(float newHeaderHeight){
	headerHeight = newHeaderHeight;
	recalculateHeight();
}

void ofxTLPage::setDefaultElementHeight(float newDefaultElementHeight){
	defaultElementHeight = newDefaultElementHeight;
}

