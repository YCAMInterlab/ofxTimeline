/*
 *  THISTimeline.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#include "ofxTimeline.h"

bool headersort(ofxTLElementHeader* a, ofxTLElementHeader* b){
	return a->getDrawRect().y < b->getDrawRect().y;
}

#define TICKER_HEIGHT 30
#define ZOOMER_HEIGHT 20

#define HEADER_HEIGHT 20 //TODO: make this variable
#define DEFAULT_ELEMENT_HEIGHT 150 //TODO: make this variable

ofxTimeline::ofxTimeline()
:	mouseoverPlayheadPosition(0),
	playbackPlayheadPosition(0),
	modalIsShown(false),
	autosave(true),
	isFrameBased(false),
	durationInFrames(100),
	durationInSeconds(0),
	currentFrameRate(0),
	isShowing(true),
	filenamePrefix("defaultTimeline_")
{
}

ofxTimeline::~ofxTimeline(){
	//TODO: cleanup!
}

void ofxTimeline::setup(){
	ticker = new ofxTLTicker();
	ticker->setup();
	ticker->setDrawRect(ofRectangle(0,0, ofGetWidth(), TICKER_HEIGHT));
	
	zoomer = new ofxTLZoomer();
	zoomer->setXMLFileName(filenamePrefix + "zoomer.xml");
	zoomer->setup();
	zoomer->setDrawRect(ofRectangle(0, TICKER_HEIGHT, ofGetWidth(), ZOOMER_HEIGHT));
		
	elements["ticker"] = ticker;
	elements["zoomer"] = zoomer;
						
	ofAddListener(ofEvents.mouseMoved, this, &ofxTimeline::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &ofxTimeline::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &ofxTimeline::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &ofxTimeline::mouseDragged);
	
	ofAddListener(ofEvents.keyPressed, this, &ofxTimeline::keyPressed);
	ofAddListener(ofEvents.windowResized, this, &ofxTimeline::windowResized);
	
	ofAddListener(ofxTLEvents.viewNeedsResize, this, &ofxTimeline::viewNeedsResize);
	
	loadElementPositions();
}

#pragma mark CONFIGURATION
void ofxTimeline::toggleShow(){
	
}

void ofxTimeline::togglePlay(){
}


void ofxTimeline::enable(){
}

void ofxTimeline::disable(){
}


void ofxTimeline::setDuration(int frames){
	ticker->setDuration(frames);
}

void ofxTimeline::setDuration(float seconds){
	ticker->setDuration(seconds);
}

void ofxTimeline::setFrameRate(int framerate){
	ticker->setFrameRate(framerate);
}

void ofxTimeline::setAutosave(bool autosave){
}

#pragma mark EVENTS
void ofxTimeline::mousePressed(ofMouseEventArgs& args){

	for(int i = 0; i < headers.size(); i++){
		headers[i]->mousePressed(args);
		elements[headers[i]->name]->mousePressed(args);
	}
	zoomer->mousePressed(args);
}

void ofxTimeline::mouseMoved(ofMouseEventArgs& args){
	ticker->mouseMoved(args);
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseMoved(args);
		elements[headers[i]->name]->mouseMoved(args);
	}
	zoomer->mouseMoved(args);
}

void ofxTimeline::mouseDragged(ofMouseEventArgs& args){
	ticker->mouseDragged(args);
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseDragged(args);
		elements[headers[i]->name]->mouseDragged(args);
	}
	zoomer->mouseDragged(args);
}

void ofxTimeline::mouseReleased(ofMouseEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		headers[i]->mouseReleased(args);
		elements[headers[i]->name]->mouseReleased(args);
	}	
	zoomer->mouseReleased(args);
}

void ofxTimeline::keyPressed(ofKeyEventArgs& args){
	for(int i = 0; i < headers.size(); i++){
		elements[headers[i]->name]->keyPressed(args);
	}
	zoomer->keyPressed(args);
}

void ofxTimeline::windowResized(ofResizeEventArgs& args){
	recalculateBoundingRects();
}

#pragma mark DRAWING

void ofxTimeline::viewNeedsResize(ofEventArgs& args){
	recalculateBoundingRects();
}

void ofxTimeline::recalculateBoundingRects(){
	
	//sort(headers.begin(), headers.end(), headersort);
	
//	for(int i = 0; i < headers.size(); i++){
//		cout << "	PRE RECALC header " << i << " is " << headers[i]->name << " y " << headers[i]->getDrawRect().y << " height " << headers[i]->getDrawRect().height << endl;
//	}

	ticker->setDrawRect( ofRectangle(0, 0, ofGetWidth(), TICKER_HEIGHT));
	float currentY = TICKER_HEIGHT;
	for(int i = 0; i < headers.size(); i++){
		ofRectangle thisHeader = headers[i]->getDrawRect();
		ofRectangle nextHeader = (i == headers.size()-1) ? zoomer->getDrawRect() : headers[i+1]->getDrawRect();
		ofRectangle elementRectangle = elements[ headers[i]->name ]->getDrawRect();
		
		float startY = thisHeader.y+thisHeader.height;
		float endY = startY + elementRectangle.height;
		
		thisHeader.width = ofGetWidth();
		thisHeader.y = currentY;
		headers[i]->setDrawRect(thisHeader);
		elementRectangle.y = startY;
		elementRectangle.width = ofGetWidth();
		
		//elementRectangle = ofRectangle(0, startY, ofGetWidth(), endY - startY);
//		cout << "element rectangle for " << headers[i]->name << " is " << elementRectangle.x << " " << elementRectangle.y << " " << elementRectangle.width << " " << elementRectangle.height << endl;
		elements[ headers[i]->name ]->setDrawRect( elementRectangle );
		currentY += thisHeader.height + elementRectangle.height + FOOTER_HEIGHT;
		
		savedElementPositions[headers[i]->name] = elementRectangle;
	}
	
	ofxTLElement* lastElement = elements[ headers[headers.size()-1]->name ];
//	zoomer->setDrawRect(ofRectangle(0, lastElement->getDrawRect().y+lastElement->getDrawRect().height + FOOTER_HEIGHT,
//									ofGetWidth(), ZOOMER_HEIGHT));
	zoomer->setDrawRect(ofRectangle(0, currentY, ofGetWidth(), ZOOMER_HEIGHT));
	
	ofRectangle totalDrawRect = ofRectangle(0,0,ofGetWidth(),currentY+ZOOMER_HEIGHT);
	ticker->setTotalDrawRect(totalDrawRect);
	
	if(autosave){
		saveElementPositions();
	}
	
//	for(int i = 0; i < headers.size(); i++){
//		cout << "	POST RECALC header " << i << " is " << headers[i]->name << " y " << headers[i]->getDrawRect().y << " height " << headers[i]->getDrawRect().height << endl;
//	}
	
}

void ofxTimeline::draw(){
		
	ticker->draw();
	
	for(int i = 0; i < headers.size(); i++){
		headers[i]->draw();
		elements[headers[i]->name]->draw();
	}
	
	zoomer->draw();
}

void ofxTimeline::loadElementPositions(){
	ofxXmlSettings elementPositions;
	if(elementPositions.loadFile("ofxTimeline_elementPositions.xml")){
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

void ofxTimeline::saveElementPositions(){
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
	elementPositions.saveFile("ofxTimeline_elementPositions.xml");
}

#pragma mark ELEMENT CREATORS/GETTERS/SETTERS
//can be used custom elements
void ofxTimeline::addTimelineElement(string name, ofxTLElement* element){

	//TODO: check to make sure we don't have an element with the same name

//	for(int i = 0; i < headers.size(); i++){
//		cout << "	PRE ADD header " << i << " is " << headers[i]->name << " y " << headers[i]->getDrawRect().y << " height " << headers[i]->getDrawRect().height << endl;
//	}
	
	ofxTLElementHeader* newHeader = new ofxTLElementHeader();
	newHeader->setElement(element);
	newHeader->setup();
	
//	cout << "adding " << name << " current zoomer is " << zoomer->getDrawRect().y << endl;
	
	ofRectangle newHeaderRect = ofRectangle(0, zoomer->getDrawRect().y, ofGetWidth(), HEADER_HEIGHT);
	newHeader->setDrawRect(newHeaderRect);
	newHeader->name = name;
	
	headers.push_back(newHeader);
	
	element->setup();
	element->setAutosave(autosave);
	ofRectangle drawRect;
	if(savedElementPositions.find(name) != savedElementPositions.end()){
		drawRect = savedElementPositions[name];
	}
	else {
		drawRect = ofRectangle(0, newHeaderRect.y+newHeaderRect.height, ofGetWidth(), DEFAULT_ELEMENT_HEIGHT);
	}

	element->setDrawRect(drawRect);
	element->setZoomBounds(zoomer->getViewRange());
	
	zoomer->offsetDrawRect( ofVec2f(0, HEADER_HEIGHT+DEFAULT_ELEMENT_HEIGHT) );
						   
	elements[name] = element;

//	for(int i = 0; i < headers.size(); i++){
//		cout << "	POST ADD header " << i << " is " << headers[i]->name << " y " << headers[i]->getDrawRect().y << " height " << headers[i]->getDrawRect().height << endl;
//	}
	
	recalculateBoundingRects();
}

ofxTLKeyframer* ofxTimeline::addKeyframes(string name, string xmlFileName, ofRange valueRange){

	ofxTLKeyframer*	newKeyframer = new ofxTLKeyframer();
	newKeyframer->setXMLFileName(xmlFileName);
	addTimelineElement(name, newKeyframer);
	
	return newKeyframer;
}

float ofxTimeline::getKeyframeValue(string name){
	
}

float ofxTimeline::getKeyframeValue(string name, float atTime){
}

float ofxTimeline::getKeyframeValue(string name, int atFrame){
}


ofxTLSwitcher* ofxTimeline::addSwitcher(string name, string xmlFileName){
}

bool ofxTimeline::getSwitcherOn(string name){
}

bool ofxTimeline::getSwitcherOn(string name, float atTime){
}

bool ofxTimeline::getSwitcherOn(string name, int atFrame){
}


ofxTLTrigger* ofxTimeline::addTriggers(string name, string xmlFileName){
}

string ofxTimeline::getNextTrigger(string name){
}

string ofxTimeline::getNextTrigger(string name, float atTime){
}

string ofxTimeline::getNextTrigger(string name, int atFrame){
}


string ofxTimeline::getLastTrigger(string name){
}

string ofxTimeline::getLastTrigger(string name, float atTime){
}

string ofxTimeline::getLastTrigger(string name, int atFrame){
}


