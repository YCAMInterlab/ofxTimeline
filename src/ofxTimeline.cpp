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


ofxTimeline::ofxTimeline()
:	mouseoverPlayheadPosition(0),
	playbackPlayheadPosition(0),
	width(1024),
	offset(ofVec2f(0,0)),
	modalIsShown(false),
	autosave(true),
	isFrameBased(false),
	durationInFrames(100),
	durationInSeconds(100/30.0),
	currentFrameRate(30),
	isShowing(true),
	filenamePrefix("defaultTimeline_"),
	isSetup(false)
{
}

ofxTimeline::~ofxTimeline(){
	if(isSetup){
		delete currentPage;
		for(int i = 0; i < pages.size(); i++){ 
			delete pages[i];
		}
		pages.clear();
		
		delete ticker;
		delete tabs;
		delete zoomer;
		
		ofRemoveListener(ofEvents.mouseMoved, this, &ofxTimeline::mouseMoved);
		ofRemoveListener(ofEvents.mousePressed, this, &ofxTimeline::mousePressed);
		ofRemoveListener(ofEvents.mouseReleased, this, &ofxTimeline::mouseReleased);
		ofRemoveListener(ofEvents.mouseDragged, this, &ofxTimeline::mouseDragged);
		
		ofRemoveListener(ofEvents.keyPressed, this, &ofxTimeline::keyPressed);
		ofRemoveListener(ofEvents.windowResized, this, &ofxTimeline::windowResized);
		
		ofRemoveListener(ofxTLEvents.viewNeedsResize, this, &ofxTimeline::viewNeedsResize);
		ofRemoveListener(ofxTLEvents.pageChanged, this, &ofxTimeline::pageChanged);
	}
}

void ofxTimeline::setup(){

	isSetup = true;
	
	width = ofGetWidth();

	tabs = new ofxTLPageTabs();
	tabs->setup();
	tabs->setDrawRect(ofRectangle(0, 0, width, TICKER_HEIGHT));

	ticker = new ofxTLTicker();
	ticker->setup();
	ticker->setDrawRect(ofRectangle(0, TICKER_HEIGHT, width, TICKER_HEIGHT));
	
	zoomer = new ofxTLZoomer();
	zoomer->setXMLFileName(filenamePrefix + "_zoomer.xml");
	zoomer->setup();
	zoomer->setDrawRect(ofRectangle(0, TICKER_HEIGHT*2, width, ZOOMER_HEIGHT));
		
	ofAddListener(ofEvents.mouseMoved, this, &ofxTimeline::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &ofxTimeline::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &ofxTimeline::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &ofxTimeline::mouseDragged);
	
	ofAddListener(ofEvents.keyPressed, this, &ofxTimeline::keyPressed);
	ofAddListener(ofEvents.windowResized, this, &ofxTimeline::windowResized);
	
	ofAddListener(ofxTLEvents.viewNeedsResize, this, &ofxTimeline::viewNeedsResize);
	ofAddListener(ofxTLEvents.pageChanged, this, &ofxTimeline::pageChanged);

	addPage("_defaultPage", true);
	
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

void ofxTimeline::setAutosave(bool doAutosave){
	autosave = doAutosave;
}

void ofxTimeline::setOffset(ofVec2f newOffset){
	offset = newOffset;
	updatePagePositions();
}

void ofxTimeline::setWidth(float newWidth){
	width = newWidth;
	updatePagePositions();
}

void ofxTimeline::updatePagePositions(){
	ofVec2f pageOffset = offset + ofVec2f(0, ticker->getDrawRect().y+ticker->getDrawRect().height);
	for(int i = 0; i < pages.size(); i++){
		pages[i]->setContainer(pageOffset, width);
	}	
	
	currentPage->recalculateHeight();
}

#pragma mark EVENTS
void ofxTimeline::mousePressed(ofMouseEventArgs& args){
	currentPage->mousePressed(args);
	zoomer->mousePressed(args);
}

void ofxTimeline::mouseMoved(ofMouseEventArgs& args){
	ticker->mouseMoved(args);
	currentPage->mouseMoved(args);
	zoomer->mouseMoved(args);
}

void ofxTimeline::mouseDragged(ofMouseEventArgs& args){
	ticker->mouseDragged(args);
	currentPage->mouseDragged(args);
	zoomer->mouseDragged(args);
}

void ofxTimeline::mouseReleased(ofMouseEventArgs& args){
	tabs->mouseReleased(args);
	currentPage->mouseReleased(args);
	zoomer->mouseReleased(args);
}

void ofxTimeline::keyPressed(ofKeyEventArgs& args){
	currentPage->keyPressed(args);
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
	
	tabs->setDrawRect( ofRectangle(offset.x, offset.y, width, TICKER_HEIGHT) );
	ticker->setDrawRect( ofRectangle(offset.x, offset.y+TICKER_HEIGHT, width, TICKER_HEIGHT) );
	updatePagePositions();

	zoomer->setDrawRect(ofRectangle(offset.x, offset.y+currentPage->getComputedHeight()+TICKER_HEIGHT*2, width, ZOOMER_HEIGHT));
	ofRectangle totalDrawRect = ofRectangle(offset.x, offset.y+TICKER_HEIGHT*2,
											width,currentPage->getComputedHeight()+ZOOMER_HEIGHT);
	ticker->setTotalDrawRect(totalDrawRect);	
}


void ofxTimeline::pageChanged(ofxTLPageEventArgs& args){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i]->getName() == args.currentPageName){
			currentPage = pages[i];
			recalculateBoundingRects();
			
			return;
		}
	}
	
	ofLogError("ofxTimeline -- Tabbed to nonexistence page " + args.currentPageName);
}

void ofxTimeline::draw(){	
	tabs->draw();
	ticker->draw();
	currentPage->draw();
	zoomer->draw();
}

#pragma mark ELEMENT CREATORS/GETTERS/SETTERS
void ofxTimeline::addPage(string name, bool makeCurrent){
	if(name == ""){
		ofLogError("ofxTimeline -- Cannot add page with an empty name.");
		return;
	}
	
	for(int i = 0; i < pages.size(); i++){
		if(name == pages[i]->getName()){
			ofLogError("ofxTimeline -- Page " + name + " already exists");
			return;
		}
	}
	
	ofxTLPage* newPage = new ofxTLPage();
	newPage->setName(name);
	newPage->setup();
	newPage->setAutosave(autosave);
	tabs->addPage(name);
	
	pages.push_back(newPage);
	if(makeCurrent){
		tabs->selectPage(name);
	}
}

void ofxTimeline::setCurrentPage(string name){
	for(int i = 0; i < pages.size(); i++){
		if(name == pages[i]->getName()){
			currentPage = pages[i];
			return;
		}
	}
	
	ofLogError("ofxTimeline -- Page " + name + " not found");
}

void ofxTimeline::setCurrentPage(int index){
	if(index >= pages.size()){
		ofLogError("ofxTimeline -- Page at index " + ofToString(index) + " does not exist");
		return;
	}
	currentPage = pages[index];
	
}

//can be used custom elements
void ofxTimeline::addTimelineElement(string name, ofxTLElement* element){
	currentPage->addElement(name, element);		
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

ofxTLImageSequence* ofxTimeline::addImageSequence(string name){
	ofFileDialogResult result = ofSystemLoadDialog("Load Sequence", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		addImageSequence(name, result.getPath());
	}
}


ofxTLImageSequence* ofxTimeline::addImageSequence(string name, string directory){
	ofxTLImageSequence*	newImageSequence = new ofxTLImageSequence();
	newImageSequence->loadSequence(directory);
	addTimelineElement(name, newImageSequence);
	return newImageSequence;	
}

ofImage* ofxTimeline::getImage(string name){
}

ofImage* ofxTimeline::getImage(string name, float atTime){
}

ofImage* ofxTimeline::getImage(string name, int atFrame){
}

