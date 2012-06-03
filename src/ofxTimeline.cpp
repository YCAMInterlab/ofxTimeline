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

#include "ofxTimeline.h"
#include "ofxTLUtils.h"

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
	durationInSeconds(100.0f/30.0f),
	isShowing(true),
	filenamePrefix("defaultTimeline_"),
	isSetup(false),
	usingEvents(false),
	isPlaying(false),
	snappingEnabled(false),
	movePlayheadOnPaste(true),
	movePlayheadOnDrag(false),
	inoutRange(ofRange(0.0,1.0)),
	currentPage(NULL)
{
}

ofxTimeline::~ofxTimeline(){
	if(isSetup){
		for(int i = 0; i < pages.size(); i++){ 
			delete pages[i];
		}
		pages.clear();
		
		delete ticker;
		delete tabs;
		delete zoomer;
		
		ofRemoveListener(ofxTLEvents.viewWasResized, this, &ofxTimeline::viewWasResized);
		ofRemoveListener(ofxTLEvents.pageChanged, this, &ofxTimeline::pageChanged);
		
	}
}

void ofxTimeline::setup(){

	isSetup = true;
	
	width = ofGetWidth();

	tabs = new ofxTLPageTabs();
	tabs->setTimeline(this);
	tabs->setup();
	tabs->setDrawRect(ofRectangle(offset.x, offset.y, width, TICKER_HEIGHT));

	ticker = new ofxTLTicker();
	ticker->setTimeline(this);
	ticker->setup();
	ticker->setDrawRect(ofRectangle(offset.x, offset.y+TICKER_HEIGHT, width, TICKER_HEIGHT));
	
	zoomer = new ofxTLZoomer();
	zoomer->setTimeline(this);
	zoomer->setXMLFileName(filenamePrefix + "_zoomer.xml");
	zoomer->setup();
	zoomer->setDrawRect(ofRectangle(offset.y, offset.y+TICKER_HEIGHT*2, width, ZOOMER_HEIGHT));
	
	colors.loadColors();
	
	enable();
	
	ofAddListener(ofxTLEvents.viewWasResized, this, &ofxTimeline::viewWasResized);
	ofAddListener(ofxTLEvents.pageChanged, this, &ofxTimeline::pageChanged);

	//You can change this name by calling setPageName()
	addPage("_defaultPage", true);
	
}

void ofxTimeline::loadElementsFromFolder(string folderPath){
    for(int i = 0; i < pages.size(); i++){
        pages[i]->loadElementsFromFolder(folderPath);
    }
}

#pragma mark CONFIGURATION
void ofxTimeline::show(){
	isShowing = true;
}
	
void ofxTimeline::hide(){
	isShowing = false;
}

bool ofxTimeline::toggleShow(){
	isShowing = !isShowing;
	return isShowing;
}

ofxTLColors& ofxTimeline::getColors(){
	return colors;
}

void ofxTimeline::setMovePlayheadOnPaste(bool move){
	movePlayheadOnPaste = move;
}

bool ofxTimeline::getMovePlayheadOnPaste(){
	return movePlayheadOnPaste;
}

string ofxTimeline::getPasteboard(){
	return pasteboard;
}

void ofxTimeline::setMovePlayheadOnDrag(bool movePlayhead){
	movePlayheadOnDrag = movePlayhead;
}

bool ofxTimeline::getMovePlayheadOnDrag(){
	return movePlayheadOnDrag;
}

ofxTLPlaybackEventArgs ofxTimeline::createPlaybackEvent(){
	ofxTLPlaybackEventArgs args;
	args.frameBased = isFrameBased;
	args.durationInFrames = durationInFrames;
	args.durationInSeconds = durationInSeconds;
	args.currentTime = currentTime;
	args.currentFrame = currentFrame;
	args.currentPercent = getPercentComplete();
	return args;
}
//internal elements call this when the value has changed
void ofxTimeline::flagUserChangedValue(){
	userChangedValue = true;
}

//this returns and clears the flag, generally call once per frame
bool ofxTimeline::getUserChangedValue(){
	bool hasChanged = userChangedValue;
    userChangedValue = false;
    return hasChanged;
}

void ofxTimeline::play(){
	if(!isPlaying){
		ofAddListener(ofEvents().update, this, &ofxTimeline::update);
		isPlaying = true;
		if (isFrameBased) {
			playbackStartFrame = ofGetFrameNum() - currentFrame;
		}
		else{
			playbackStartTime = ofGetElapsedTimef() - currentTime;
		}		
		ofxTLPlaybackEventArgs args = createPlaybackEvent();
		ofNotifyEvent(ofxTLEvents.playbackStarted, args);
	}
}

void ofxTimeline::stop(){
	if(isPlaying){
		isPlaying = false;
		ofRemoveListener(ofEvents().update, this, &ofxTimeline::update);
		ofxTLPlaybackEventArgs args = createPlaybackEvent();
		ofNotifyEvent(ofxTLEvents.playbackEnded, args);
	}
}

bool ofxTimeline::togglePlay(){
	if(isPlaying){
		stop();
	}
	else{
		if(loopType == OF_LOOP_NONE && getPercentComplete() == 1.0){
			setPercentComplete(0.0);
		}
		play();
	}
	return isPlaying;
}

bool ofxTimeline::getIsPlaying(){
	return isPlaying;
}

void ofxTimeline::setCurrentFrame(int newFrame){
	if(!isFrameBased){
		ofLogWarning("ofxTimeline -- setting current frame on a timebased timline has no effect.");
	}
//	cout << "setting frame to " << newFrame << endl;
	currentFrame = newFrame;
}

void ofxTimeline::setPercentComplete(float percent){
	if(isFrameBased){
		currentFrame = durationInFrames*percent;
	}
	else{
		currentTime = percent*durationInSeconds;
	}
}

void ofxTimeline::setCurrentTime(float time){
	if(isFrameBased){
		ofLogWarning("ofxTimeline -- setting current time on a framebased timeline has no effect.");
	}
	currentTime = time;
}

bool ofxTimeline::getIsFrameBased(){
	return isFrameBased;
}

int ofxTimeline::getCurrentFrame(){
	if(!isFrameBased){
		ofLogWarning("ofxTimeline -- getting current frame on a timebased timline will return inaccurate results.");
	}
	return currentFrame;
}

float ofxTimeline::getCurrentTime(){
	if(isFrameBased){
		ofLogWarning("ofxTimeline -- getting current time on a framebased will return inaccurate results.");
	}
	return currentTime;
}

float ofxTimeline::getPercentComplete(){
	if(isFrameBased){
		return float(currentFrame)/durationInFrames;
	}
	else{
		return currentTime / durationInSeconds;
	}
}


void ofxTimeline::setInPointAtPercent(float percent){
	inoutRange.min = percent;
}

void ofxTimeline::setOutPointAtPercent(float percent){
	inoutRange.max = percent;
}

void ofxTimeline::setInPointAtFrame(int frame){
	if(!isFrameBased){
		ofLogWarning("ofxTimeline -- setting in point by frame on timebased timeline will not work correctly.");
	}
	inoutRange.min = float(frame)/durationInFrames;
}

void ofxTimeline::setOutPointAtFrame(float frame){
	if(!isFrameBased){
		ofLogWarning("ofxTimeline -- setting out point by frame on timebased timeline will not work correctly.");
	}
	inoutRange.max = float(frame)/durationInFrames;	
}

void ofxTimeline::setInOutRange(ofRange inoutPercentRange){
	inoutRange = inoutPercentRange;
}

void ofxTimeline::setCurrentTimeToInPoint(){
	inoutRange.min = getPercentComplete();
	if(inoutRange.max < inoutRange.min){
		inoutRange.max = 1.0;
	}
}

void ofxTimeline::setCurrentTimeToOutPoint(){
	inoutRange.max = getPercentComplete();
	if(inoutRange.min > inoutRange.max){
		inoutRange.min = 0.0;
	}	
}

ofRange ofxTimeline::getInOutRange(){
	return inoutRange;
}

int ofxTimeline::getInFrame(){
	if (!isFrameBased) {
		ofLogWarning("ofxTimeline -- Getting in frame with timebased timeline");
	}
	return durationInFrames*inoutRange.min;
}

int ofxTimeline::getOutFrame(){
	if (!isFrameBased) {
		ofLogWarning("ofxTimeline -- Getting out frame with timebased timeline");
	}
	return durationInFrames*inoutRange.max;
}

float ofxTimeline::getInTime(){
	if (isFrameBased) {
		ofLogWarning("ofxTimeline -- Getting in time with framebased timeline");
	}
	return durationInSeconds*inoutRange.min;
}

float ofxTimeline::getOutTime(){
	if (isFrameBased) {
		ofLogWarning("ofxTimeline -- Getting out time with framebased timeline");
	}
	return durationInSeconds*inoutRange.max;
}


bool ofxTimeline::toggleEnabled(){
	isEnabled = !isEnabled;
	return isEnabled;
}

void ofxTimeline::enable(){
	isEnabled = true;
	enableEvents();
}

void ofxTimeline::disable(){
	isEnabled = false;
	disableEvents();
}

//clears every element
void ofxTimeline::reset(){
	for(int i = 0; i < pages.size(); i++){
        pages[i]->reset();
    }
}

void ofxTimeline::save(){
	for(int i = 0; i < pages.size(); i++){
        pages[i]->save();
    }	
}

void ofxTimeline::setDurationInFrames(int frames){
    if(frames < 1){
    	ofLogError("ofxTimeline -- Must set a positive number of frames");
        return;
    }
	durationInFrames = frames;
	isFrameBased = true;
}

void ofxTimeline::setDurationInSeconds(float seconds){
    if(seconds <= 0.){
    	ofLogError("ofxTimeline -- Must set a positive time of frames");
        return;
    }
	durationInSeconds = seconds;
	isFrameBased = false;
}

int ofxTimeline::getDurationInFrames(){
	if(!isFrameBased){
		ofLogWarning("ofxTimeline -- getting duration in frames on a timebased timline will return inaccurate results.");
	}
	return durationInFrames;
}

float ofxTimeline::getDurationInSeconds(){
	if(isFrameBased){
		ofLogWarning("ofxTimeline -- getting duration in time on a framebased will return inaccurate results.");
	}
	return durationInSeconds;
}

void ofxTimeline::setAutosave(bool doAutosave){
	autosave = doAutosave;
}

void ofxTimeline::setOffset(ofVec2f newOffset){
	offset = newOffset;
	updatePagePositions();
	//recalculateBoundingRects();
	ofEventArgs args;
	ofNotifyEvent(ofxTLEvents.viewWasResized, args);

}

void ofxTimeline::setWidth(float newWidth){
	width = newWidth;
	updatePagePositions();
//	recalculateBoundingRects();
	ofEventArgs args;
	ofNotifyEvent(ofxTLEvents.viewWasResized, args);

}

void ofxTimeline::collapseAllElements(){
	currentPage->collapseAllElements();
}

ofRectangle ofxTimeline::getDrawRect(){
	return totalDrawRect;
}

void ofxTimeline::updatePagePositions(){
	if(isSetup){
		ofVec2f pageOffset = ofVec2f(0, ticker->getDrawRect().y+ticker->getDrawRect().height);
		for(int i = 0; i < pages.size(); i++){
			pages[i]->setContainer(pageOffset, width);
		}
		currentPage->recalculateHeight();
	}
}

void ofxTimeline::setSnapping(bool snapping){
	snappingEnabled = snapping;
	for(int i = 0; i < pages.size(); i++){
		pages[i]->setSnapping(snappingEnabled);
	}
}

void ofxTimeline::unselectAll(){
	currentPage->unselectAll();
}

void ofxTimeline::toggleSnapping(){
	setSnapping(!snappingEnabled);
}

void ofxTimeline::enableSnapToBPM(float bpm){
	ticker->setBPM(bpm);
}

void ofxTimeline::enableSnapToOtherElements(bool enableSnapToOther){
	for(int i = 0; i < pages.size(); i++){
		currentPage->enableSnapToOtherElements(enableSnapToOther);
	}
}

void ofxTimeline::toggleDrawBPMGrid(){
	ticker->setDrawBPMGrid(!ticker->getDrawBPMGrid());		
}

void ofxTimeline::enableDrawBPMGrid(bool enableGrid){
	ticker->setDrawBPMGrid(enableGrid);
}

#pragma mark EVENTS
void ofxTimeline::enableEvents() {
	if (!usingEvents) {
		ofAddListener(ofEvents().mouseMoved, this, &ofxTimeline::mouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxTimeline::mousePressed);
		ofAddListener(ofEvents().mouseReleased, this, &ofxTimeline::mouseReleased);
		ofAddListener(ofEvents().mouseDragged, this, &ofxTimeline::mouseDragged);
		
		ofAddListener(ofEvents().keyPressed, this, &ofxTimeline::keyPressed);
		ofAddListener(ofEvents().windowResized, this, &ofxTimeline::windowResized);
		
		usingEvents = true;
	}
}

void ofxTimeline::disableEvents() {
	if (usingEvents) {
		ofRemoveListener(ofEvents().mouseMoved, this, &ofxTimeline::mouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &ofxTimeline::mousePressed);
		ofRemoveListener(ofEvents().mouseReleased, this, &ofxTimeline::mouseReleased);
		ofRemoveListener(ofEvents().mouseDragged, this, &ofxTimeline::mouseDragged);
		
		ofRemoveListener(ofEvents().keyPressed, this, &ofxTimeline::keyPressed);
		ofRemoveListener(ofEvents().windowResized, this, &ofxTimeline::windowResized);
		
		usingEvents = false;
	}
}

void ofxTimeline::mousePressed(ofMouseEventArgs& args){
	dragAnchorSet = false;
	ticker->mousePressed(args);
	currentPage->mousePressed(args);
	zoomer->mousePressed(args);

	currentPage->setSnapping(snappingEnabled && dragAnchorSet);
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
	ticker->mouseReleased(args);
	tabs->mouseReleased(args);
	currentPage->mouseReleased(args);
	zoomer->mouseReleased(args);
}

void ofxTimeline::keyPressed(ofKeyEventArgs& args){
//	cout << "key event " << args.key << " ctrl? " << ofGetModifierKeyControl() << endl;
	if(ofGetModifierKeyControl() && args.key == 3){ //copy
		string copyattempt = currentPage->copyRequest();
		if(copyattempt != ""){
			pasteboard = copyattempt;
		}
	}
	else if(ofGetModifierKeyControl() && args.key == 24){ //cut
		string copyattempt = currentPage->cutRequest();
		if(copyattempt != ""){
			pasteboard = copyattempt;
		}
	}
	else if(ofGetModifierKeyControl() && args.key == 22){ //paste
		if (pasteboard != "") {
			currentPage->pasteSent(pasteboard);
		}				
	}
	else if(ofGetModifierKeyControl() && args.key == 1){ //select all
		currentPage->selectAll();						
	}
	
	else{
		if(args.key >= OF_KEY_LEFT && args.key <= OF_KEY_DOWN){
			ofVec2f nudgeAmount = ofGetModifierKeyShift() ? getBigNudgePercent() : getNudgePercent();
			if(args.key == OF_KEY_UP){
				nudgeAmount.x = 0;
			}
			if(args.key == OF_KEY_DOWN){
				nudgeAmount.x = 0;
				nudgeAmount.y = -nudgeAmount.y;
			}
			if(args.key == OF_KEY_RIGHT){
				nudgeAmount.y = 0;
			}
			if(args.key == OF_KEY_LEFT){
				nudgeAmount.x = -nudgeAmount.x;
				nudgeAmount.y = 0;
			}
			currentPage->nudgeBy(nudgeAmount);

		}
		
		ticker->keyPressed(args);
		currentPage->keyPressed(args);
		zoomer->keyPressed(args);
	}
}

void ofxTimeline::windowResized(ofResizeEventArgs& args){
	recalculateBoundingRects();
}

#pragma mark DRAWING
void ofxTimeline::viewWasResized(ofEventArgs& args){
	recalculateBoundingRects();
}

void ofxTimeline::recalculateBoundingRects(){
	
	if(pages.size() > 1){
		tabs->setDrawRect(ofRectangle(offset.x, offset.y, width, TICKER_HEIGHT));
	}
	else{
		tabs->setDrawRect(ofRectangle(offset.x, offset.y, width, 0));
	}
	ticker->setDrawRect( ofRectangle(offset.x, offset.y+tabs->getDrawRect().height, width, TICKER_HEIGHT) );
	updatePagePositions();

	zoomer->setDrawRect(ofRectangle(offset.x, offset.y+currentPage->getComputedHeight()+ticker->getDrawRect().height+tabs->getDrawRect().height, width, ZOOMER_HEIGHT));
	
	ofRectangle tickerRect = ofRectangle(offset.x, offset.y+tabs->getDrawRect().height,
											width,ticker->getDrawRect().height+currentPage->getComputedHeight()+ZOOMER_HEIGHT);
	ticker->setTotalDrawRect(tickerRect);		
	
	totalDrawRect = ofRectangle(offset.x, offset.y, width, zoomer->getDrawRect().y+ZOOMER_HEIGHT - offset.y);
}


void ofxTimeline::pageChanged(ofxTLPageEventArgs& args){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i]->getName() == args.currentPageName){
			if(currentPage != NULL){
				currentPage->unselectAll();
			}
			currentPage = pages[i];
			ofEventArgs args;
			ofNotifyEvent(ofxTLEvents.viewWasResized, args);
			return;
		}
	}
		
	ofLogError("ofxTimeline -- Tabbed to nonexistence page " + args.currentPageName);
}

void ofxTimeline::setLoopType(ofLoopType newType){
	loopType = newType;
}

ofLoopType ofxTimeline::getLoopType(){
	return loopType;
}

void ofxTimeline::update(ofEventArgs& updateArgs){
	if(isFrameBased){
		currentFrame = ofGetFrameNum() - playbackStartFrame;
		if(currentFrame >= durationInFrames*inoutRange.max){
			if(loopType == OF_LOOP_NONE){
				currentFrame = durationInFrames*inoutRange.max;
				stop();
			}
			else if(loopType == OF_LOOP_NORMAL && durationInFrames > 0) {
				playbackStartFrame += getDurationInFrames() * inoutRange.span();
				currentFrame %= int(durationInFrames * inoutRange.span());
				currentFrame += durationInFrames*inoutRange.min;
				ofxTLPlaybackEventArgs args = createPlaybackEvent();
				ofNotifyEvent(ofxTLEvents.playbackLooped, args);
			}
		}
	}
	else{
		currentTime = ofGetElapsedTimef() - playbackStartTime;
		if(currentTime >= durationInSeconds*inoutRange.max){
			if(loopType == OF_LOOP_NONE){
				currentTime = durationInSeconds*inoutRange.max;
				stop();
			}
			else if(loopType == OF_LOOP_NORMAL) {
				currentTime = durationInSeconds*inoutRange.min + fmod(currentTime, durationInSeconds*inoutRange.span());
				playbackStartTime += getDurationInSeconds()*inoutRange.span();
				ofxTLPlaybackEventArgs args = createPlaybackEvent();
				ofNotifyEvent(ofxTLEvents.playbackLooped, args);
			}
		}
	}
}

void ofxTimeline::draw(){	
	if(isShowing){
		ofPushStyle();
		
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		ofEnableAlphaBlending();
		
		if (pages.size() > 1) {
			tabs->draw();			
		}
		currentPage->draw();
		zoomer->draw();
		ticker->draw();
		
		glPopAttrib();
		ofPopStyle();
	}
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
	newPage->setZoomBounds(zoomer->getViewRange());
	newPage->setTicker(ticker);
	tabs->addPage(name);

	pages.push_back(newPage);
	if(makeCurrent){
		tabs->selectPage(name);
	}
}

void ofxTimeline::setPageName(string newName){
	tabs->changeName(currentPage->getName(), newName);
	currentPage->setName( newName );
	currentPage->loadElementPositions();
}

void ofxTimeline::setCurrentPage(string name){
	tabs->selectPage(name);
/*
	for(int i = 0; i < pages.size(); i++){
		if(name == pages[i]->getName()){
			
			currentPage = pages[i];
			return;
		}
	}
	ofLogError("ofxTimeline -- Page " + name + " not found");
 */
	
}

void ofxTimeline::setCurrentPage(int index){
//	if(index >= pages.size()){
//		ofLogError("ofxTimeline -- Page at index " + ofToString(index) + " does not exist");
//		return;
//	}
//	currentPage = pages[index];
	
	tabs->selectPage(index);
}

//can be used to add custom elements
void ofxTimeline::addElement(string name, ofxTLElement* element){
	element->setTimeline( this );
	element->setName( name );
	currentPage->addElement(name, element);		
	elementNameToPage[name] = currentPage;
//	recalculateBoundingRects();
	ofEventArgs args;
	ofNotifyEvent(ofxTLEvents.viewWasResized, args);

}

ofxTLKeyframer* ofxTimeline::addKeyframes(string name, string xmlFileName, ofRange valueRange, float defaultValue){

	ofxTLKeyframer*	newKeyframer = new ofxTLKeyframer();
	newKeyframer->setCreatedByTimeline(true);
	newKeyframer->setValueRange(valueRange, defaultValue);
	newKeyframer->setXMLFileName(xmlFileName);
	addElement(name, newKeyframer);
	
	return newKeyframer;
}

float ofxTimeline::getKeyframeValue(string name){
	if(isFrameBased){
		return getKeyframeValue(name, currentFrame);
	}
	else {
		return getKeyframeValue(name, currentTime);
	}
}

float ofxTimeline::getKeyframeValue(string name, float atTime){
	if(elementNameToPage.find(name) == elementNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return 0.0;
	}
	ofxTLKeyframer* keyframer = (ofxTLKeyframer*)elementNameToPage[name]->getElement(name);
	if(keyframer == NULL){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return 0.0;
	}
	return keyframer->getValueAtPercent(atTime/durationInSeconds);
}

float ofxTimeline::getKeyframeValue(string name, int atFrame){
	if(elementNameToPage.find(name) == elementNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return 0.0;
	}
	
	ofxTLKeyframer* keyframer = (ofxTLKeyframer*)elementNameToPage[name]->getElement(name);
	if(keyframer == NULL){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return 0.0;
	}
	return keyframer->getValueAtPercent(1.0*currentFrame/durationInFrames);
}

ofxTLElement* ofxTimeline::getElement(string name){
	if(elementNameToPage.find(name) == elementNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return NULL;
	}
	return elementNameToPage[name]->getElement(name);
}

ofxTLSwitcher* ofxTimeline::addSwitcher(string name, string xmlFileName){
	ofxTLSwitcher* newSwitcher = new ofxTLSwitcher();
	newSwitcher->setCreatedByTimeline(true);
	newSwitcher->setXMLFileName(xmlFileName);
	addElement(name, newSwitcher);
	
	return newSwitcher;
}

bool ofxTimeline::getSwitcherOn(string name){
	if(elementNameToPage.find(name) == elementNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return false;
	}
	
	ofxTLSwitcher* switcher = (ofxTLSwitcher*)elementNameToPage[name]->getElement(name);
	if(switcher == NULL){
		ofLogError("ofxTimeline -- Couldn't find switcher element " + name);
		return false;
	}
	return switcher->isOn(getPercentComplete());
}

bool ofxTimeline::getSwitcherOn(string name, float atTime){
	if(elementNameToPage.find(name) == elementNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return false;
	}	
	ofxTLSwitcher* switcher = (ofxTLSwitcher*)elementNameToPage[name]->getElement(name);
	if(switcher == NULL){
		ofLogError("ofxTimeline -- Couldn't find switcher element " + name);
		return false;
	}
	if(isFrameBased){
		ofLogError("ofxTimeline -- Getting time based switcher status on a framebased timeline will return in accurate results.");
	}
	
	return switcher->isOn(atTime/durationInSeconds);	
}

bool ofxTimeline::getSwitcherOn(string name, int atFrame){
	if(elementNameToPage.find(name) == elementNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find element " + name);
		return false;
	}
	
	ofxTLSwitcher* switcher = (ofxTLSwitcher*)elementNameToPage[name]->getElement(name);
	if(switcher == NULL){
		ofLogError("ofxTimeline -- Couldn't find switcher element " + name);
		return false;
	}
	if(!isFrameBased){
		ofLogError("ofxTimeline -- Getting frame based switcher status on a timebased timeline will return in accurate results.");
	}
	return switcher->isOn(float(atFrame)/durationInFrames);	
}

ofxTLTrigger* ofxTimeline::addTriggers(string name, string xmlFileName){
	ofxTLTrigger* newTrigger = new ofxTLTrigger();
	newTrigger->setCreatedByTimeline(true);
	newTrigger->setXMLFileName(xmlFileName);
	addElement(name, newTrigger);

	return newTrigger;
}

string ofxTimeline::getNextTrigger(string name){
	return "";
}

string ofxTimeline::getNextTrigger(string name, float atTime){
	return "";
}

string ofxTimeline::getNextTrigger(string name, int atFrame){
	return "";
}

string ofxTimeline::getLastTrigger(string name){
	return "";
}

string ofxTimeline::getLastTrigger(string name, float atTime){
	return "";
}

string ofxTimeline::getLastTrigger(string name, int atFrame){
	return "";
}

ofxTLImageSequence* ofxTimeline::addImageSequence(string name){
	ofFileDialogResult result = ofSystemLoadDialog("Load Sequence", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		return addImageSequence(name, result.getPath());
	}
	return NULL;
}

ofxTLImageSequence* ofxTimeline::addImageSequence(string name, string directory){
	ofxTLImageSequence*	newImageSequence = new ofxTLImageSequence();
	newImageSequence->setCreatedByTimeline(true);
	newImageSequence->loadSequence(directory);
	addElement(name, newImageSequence);
	return newImageSequence;	
}

ofImage* ofxTimeline::getImage(string name){
	return NULL;
}

ofImage* ofxTimeline::getImage(string name, float atTime){
	return NULL;
}

ofImage* ofxTimeline::getImage(string name, int atFrame){
	return NULL;
}

string ofxTimeline::formatTime(float time){
	if(isFrameBased){
		ofLogError("ofxTimeline -- formatting time for framebased timeline doesn't make sense");
	}
	
	char out[1024];
	int millis,seconds,minutes,hours;
	millis = int(time * 10000) % 10000;
	seconds = int(time) % 60;
	minutes = int(time/60) % 60;
	hours = int(time/60/60);
	sprintf(out, "%02d:%02d:%02d:%04d", hours, minutes,seconds,(millis+1));
	//truncate for shorter timelines
	if(minutes == 0){
		return string(out).substr(6, 7);
	}
	if(hours == 0){
		return string(out).substr(3, 10);
	}
	return string(out);
}

void ofxTimeline::setDragAnchor(float dragAnchor){
	globalDragAnchor = dragAnchor;
	for(int i = 0; i < pages.size(); i++){
		pages[i]->setDragAnchor( dragAnchor );
	}
	dragAnchorSet = true;
}

float ofxTimeline::getDragAnchor(){
	return globalDragAnchor;
}

ofVec2f ofxTimeline::getNudgePercent(){
	return ofVec2f(zoomer->getViewRange().span()*.001, .001);
}

ofVec2f ofxTimeline::getBigNudgePercent(){
	return ofVec2f(zoomer->getViewRange().span()*.02, 0.02);	
}
