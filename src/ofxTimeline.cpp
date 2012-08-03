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

//increments to keep auto generated names unique
static int timelineNumber = 0;

bool headersort(ofxTLTrackHeader* a, ofxTLTrackHeader* b){
	return a->getDrawRect().y < b->getDrawRect().y;
}

#define TICKER_HEIGHT 20
#define ZOOMER_HEIGHT 14
#define INOUT_HEIGHT 7

ofxTimeline::ofxTimeline()
:	width(1024),
	offset(ofVec2f(0,0)),
	autosave(true),
	isFrameBased(false),
	timelineHasFocus(false),
	showTicker(true), 
	showInoutControl(true),
	showZoomer(true),
	durationInSeconds(100.0f/30.0f),
	isShowing(true),
	isSetup(false),
	usingEvents(false),
	isPlaying(false),
	isEnabled(false),
	dragAnchorSet(false),
	snappingEnabled(false),
	globalDragAnchor(0),
	movePlayheadOnPaste(true),
	movePlayheadOnDrag(false),
	inoutRange(ofRange(0.0,1.0)),
	currentPage(NULL),
	modalTrack(NULL),
	timeControl(NULL),
	loopType(OF_LOOP_NONE),
	lockWidthToWindow(true),
	currentTime(0.0)
{
}

ofxTimeline::~ofxTimeline(){
	if(isSetup){
		ofRemoveListener(timelineEvents.viewWasResized, this, &ofxTimeline::viewWasResized);
		ofRemoveListener(timelineEvents.pageChanged, this, &ofxTimeline::pageChanged);

        //TODO: move to shared pointers 
        //this breaks timelines that are statically declared because 
        //there is no copy/assignment constructor
		for(int i = 0; i < pages.size(); i++){ 
			delete pages[i];
		}
		pages.clear();
		
		delete ticker;
		delete tabs;
		delete zoomer;
		
        
        cout << "*** DESTRUCTORING**" << endl;
	}
}

void ofxTimeline::setup(){

	isSetup = true;
	
	width = ofGetWidth();

	tabs = new ofxTLPageTabs();
	tabs->setTimeline(this);
	tabs->setup();
	tabs->setDrawRect(ofRectangle(offset.x, offset.y, width, TICKER_HEIGHT));

    inoutTrack = new ofxTLInOut();
    inoutTrack->setTimeline(this);
    inoutTrack->setDrawRect(ofRectangle(offset.x, tabs->getBottomEdge(), width, INOUT_HEIGHT));
    
	ticker = new ofxTLTicker();
	ticker->setTimeline(this);
    //todo save ticker positin
	ticker->setup();
	ticker->setDrawRect(ofRectangle(offset.x, inoutTrack->getBottomEdge(), width, TICKER_HEIGHT));
	
	zoomer = new ofxTLZoomer();
	zoomer->setTimeline(this);
	zoomer->setDrawRect(ofRectangle(offset.y, ticker->getBottomEdge(), width, ZOOMER_HEIGHT));
	
    setName("timeline" + ofToString(timelineNumber++));
    
	colors.load();

	enable();
    
	ofAddListener(timelineEvents.viewWasResized, this, &ofxTimeline::viewWasResized);
	ofAddListener(timelineEvents.pageChanged, this, &ofxTimeline::pageChanged);

    //You can change this name by calling setPageName()
	addPage("Page One", true);
}

void ofxTimeline::setName(string newName){
    if(newName != name){
        cout << "setting name to " << newName << endl;
	    name = newName;
        inoutTrack->setXMLFileName(name + "_inout.xml");
        inoutTrack->setup();
        
        zoomer->setXMLFileName(name + "_zoomer.xml");
        zoomer->setup();
    }
}

string ofxTimeline::getName(){
    return name;
}

void ofxTimeline::loadTracksFromFolder(string folderPath){
    for(int i = 0; i < pages.size(); i++){
        pages[i]->loadTracksFromFolder(folderPath);
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

void ofxTimeline::setShowTimeControls(bool shouldShowTimeControls){
    showTicker = showInoutControl = showZoomer = shouldShowTimeControls;
    recalculateBoundingRects();
}

void ofxTimeline::setShowTicker(bool shouldShowTicker){
    showTicker = shouldShowTicker;
    recalculateBoundingRects();
}

void ofxTimeline::setShowInoutControl(bool shouldShowInoutControl){
    showInoutControl = shouldShowInoutControl;
    recalculateBoundingRects();
}

void ofxTimeline::setShowZoomer(bool shouldShowZoomer){
    showZoomer = shouldShowZoomer;
    recalculateBoundingRects();
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
    args.sender = this;
	args.durationInFrames = timecode.frameForSeconds(durationInSeconds);
	args.durationInSeconds = durationInSeconds;
	args.currentTime = currentTime;
	args.currentFrame = getCurrentFrame();
	args.currentPercent = getPercentComplete();
	return args;
}

//internal elements call this when the value has changed, can call repeated
void ofxTimeline::flagUserChangedValue(){
	userChangedValue = true;
}

//this returns and clears the flag, generally call once per frame
bool ofxTimeline::getUserChangedValue(){
	bool hasChanged = userChangedValue;
    userChangedValue = false;
    return hasChanged;
}

void ofxTimeline::flagTrackModified(ofxTLTrack* track){
	flagUserChangedValue();
    if(autosave){
        track->save();
    }
}

void ofxTimeline::play(){

    if(!isEnabled){
        return;
    }

    if(timeControl != NULL){
        timeControl->play();
        return;
    }
	if(!getIsPlaying()){
        if(isDone()){
            setPercentComplete(0.0);
        }
		ofAddListener(ofEvents().update, this, &ofxTimeline::update);
		isPlaying = true;
        currentTime = ofClamp(currentTime, getInTime(), getOutTime());
        
        playbackStartTime = timer.getAppTime() - currentTime;
        playbackStartFrame = ofGetFrameNum() - timecode.frameForSeconds(currentTime);        
		ofxTLPlaybackEventArgs args = createPlaybackEvent();
		ofNotifyEvent(timelineEvents.playbackStarted, args);
	}
}

void ofxTimeline::stop(){

    if(!isEnabled){
        return;
    }

    if(timeControl != NULL){
        timeControl->stop();
        return;
    }

	if(getIsPlaying()){
        isPlaying = false;
        ofRemoveListener(ofEvents().update, this, &ofxTimeline::update);
        ofxTLPlaybackEventArgs args = createPlaybackEvent();
        ofNotifyEvent(timelineEvents.playbackEnded, args);
	}
}

bool ofxTimeline::togglePlay(){
    if(!isEnabled){
        return;
    }
    
    if(timeControl != NULL){
        timeControl->togglePlay();
        return;
    }

	if(getIsPlaying()){
		stop();
	}
	else{
		play();
	}
	return getIsPlaying();
}

bool ofxTimeline::getIsPlaying(){
	return timeControl != NULL ? timeControl->isPlaying() : isPlaying;
}

void ofxTimeline::setCurrentFrame(int newFrame){
    currentTime = timecode.secondsForFrame(newFrame);
}

void ofxTimeline::setPercentComplete(float percent){
    currentTime = percent*durationInSeconds;
}

void ofxTimeline::setCurrentTime(float time){
	currentTime = time;
}

void ofxTimeline::setFrameRate(float fps){
    //TODO: Retime track contents?
	timecode.setFPS(fps);    
}
void ofxTimeline::setFrameBased(bool frameBased){
    isFrameBased = frameBased;
}

bool ofxTimeline::getIsFrameBased(){
	return isFrameBased;
}

int ofxTimeline::getCurrentFrame(){
    return timecode.frameForSeconds(currentTime);
}

float ofxTimeline::getCurrentTime(){
	return currentTime;
}

float ofxTimeline::getPercentComplete(){
    return currentTime / durationInSeconds;
}

//TODO: santize in/out 
//TODO: all should call overloaded function so it's easier to subclass
void ofxTimeline::setInPointAtPlayhead(){
    setInPointAtTime(currentTime);
}

void ofxTimeline::setOutPointAtPlayhead(){
    setOutPointAtTime(currentTime);
}

void ofxTimeline::setInPointAtPercent(float percent){
	inoutRange.min = percent;
}

void ofxTimeline::setOutPointAtPercent(float percent){
	inoutRange.max = percent;
}

void ofxTimeline::setInPointAtFrame(int frame){
    inoutRange.min = timecode.secondsForFrame(frame) / durationInSeconds;
}

void ofxTimeline::setOutPointAtFrame(float frame){
    inoutRange.max = timecode.secondsForFrame(frame) / durationInSeconds;
}

void ofxTimeline::setInPointAtTime(float time){
	inoutRange.min = time/durationInSeconds;	    
}

void ofxTimeline::setOutPointAtTime(float time){
    inoutRange.max = time/durationInSeconds;
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
	return timecode.frameForSeconds(getInTime());
}

int ofxTimeline::getOutFrame(){
    return timecode.frameForSeconds(getOutTime());
}

float ofxTimeline::getInTime(){
	return durationInSeconds*inoutRange.min;
}

float ofxTimeline::getOutTime(){
	return durationInSeconds*inoutRange.max;
}

bool ofxTimeline::toggleEnabled(){
	isEnabled = !isEnabled;
	return isEnabled;
}

void ofxTimeline::enable(){
    if(!isEnabled){
		isEnabled = true;
		enableEvents();
    }
}

void ofxTimeline::disable(){
    if(isEnabled){
		isEnabled = false;
		disableEvents();
    }
}

//clears every element
void ofxTimeline::clear(){
	for(int i = 0; i < pages.size(); i++){
        pages[i]->clear();
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
    durationInSeconds = timecode.secondsForFrame(frames);
}

void ofxTimeline::setDurationInSeconds(float seconds){
    if(seconds <= 0.){
    	ofLogError("ofxTimeline -- Must set a positive time of frames");
        return;
    }
	durationInSeconds = seconds;
}

int ofxTimeline::getDurationInFrames(){
    return timecode.frameForSeconds(durationInSeconds);
}

float ofxTimeline::getDurationInSeconds(){
	return durationInSeconds;
}

string ofxTimeline::getDurationInTimecode(){
    return timecode.timecodeForSeconds(durationInSeconds);
}

void ofxTimeline::setAutosave(bool doAutosave){
	autosave = doAutosave;
}

void ofxTimeline::setOffset(ofVec2f newOffset){
	offset = newOffset;
	updatePagePositions();
	//recalculateBoundingRects();
	ofEventArgs args;
	ofNotifyEvent(timelineEvents.viewWasResized, args);

}

void ofxTimeline::setLockWidthToWindow(bool lockWidth){
    lockWidthToWindow = lockWidth;
    if(width != ofGetWidth()){
        recalculateBoundingRects();
    }
}

bool ofxTimeline::getLockWidthToWindow(){
    return lockWidthToWindow;
}

void ofxTimeline::setWidth(float newWidth){
	width = newWidth;
	updatePagePositions();
//	recalculateBoundingRects();
	ofEventArgs args;
	ofNotifyEvent(events().viewWasResized, args);

}

void ofxTimeline::collapseAllTracks(){
	currentPage->collapseAllTracks();
}

ofRectangle ofxTimeline::getDrawRect(){
	return totalDrawRect;
}

ofVec2f ofxTimeline::getTopLeft(){
    return ofVec2f(totalDrawRect.x, totalDrawRect.y);
}

ofVec2f ofxTimeline::getTopRight(){
    return ofVec2f(totalDrawRect.x+totalDrawRect.width, totalDrawRect.y);
}

ofVec2f ofxTimeline::getBottomLeft(){
	return ofVec2f(totalDrawRect.x, totalDrawRect.y+totalDrawRect.height);    
}

ofVec2f ofxTimeline::getBottomRight(){
	return ofVec2f(totalDrawRect.x+totalDrawRect.width, totalDrawRect.y+totalDrawRect.height);    
}

void ofxTimeline::updatePagePositions(){
	if(isSetup){
		ofVec2f pageOffset = ofVec2f(0, inoutTrack->getBottomEdge());
		for(int i = 0; i < pages.size(); i++){
			pages[i]->setContainer(pageOffset, width);
		}
		currentPage->recalculateHeight();
	}
}

void ofxTimeline::setBPM(float bpm) {
    ticker->setBPM(bpm);
}

float ofxTimeline::getBPM() {
	return ticker->getBPM();
}

void ofxTimeline::setSnapping(bool snapping){
	snappingEnabled = snapping;
	for(int i = 0; i < pages.size(); i++){
		pages[i]->setSnapping(snappingEnabled);
	}
}

void ofxTimeline::presentedModalContent(ofxTLTrack* newModalTrack){
    modalTrack = newModalTrack;
}

void ofxTimeline::dismissedModalContent(){
	modalTrack = NULL;    
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

void ofxTimeline::enableSnapToOtherKeyframes(bool enableSnapToOther){
	for(int i = 0; i < pages.size(); i++){
		currentPage->enableSnapToOtherTracks(enableSnapToOther);
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
    
    if(modalTrack != NULL){
    	modalTrack->mousePressed(args);
        return;
    }
    
	dragAnchorSet = false;
    bool focus = getDrawRect().inside(args.x, args.y);
	if(focus && !timelineHasFocus){
    	currentPage->timelineGainedFocus();    
    }
    else if(!focus && timelineHasFocus){
        currentPage->timelineLostFocus();
    }
    focus = timelineHasFocus;
    inoutTrack->mousePressed(args);
	ticker->mousePressed(args);
	currentPage->mousePressed(args);
	zoomer->mousePressed(args);

	currentPage->setSnapping(snappingEnabled && dragAnchorSet);
}

void ofxTimeline::mouseMoved(ofMouseEventArgs& args){
    if(modalTrack != NULL){
    	modalTrack->mouseMoved(args);
        return;
    }
    
    inoutTrack->mouseMoved(args);
	ticker->mouseMoved(args);
	currentPage->mouseMoved(args);
	zoomer->mouseMoved(args);
}

void ofxTimeline::mouseDragged(ofMouseEventArgs& args){
    if(modalTrack != NULL){
    	modalTrack->mouseDragged(args, false);
        return;
    }
    
	inoutTrack->mouseDragged(args);
	ticker->mouseDragged(args);
	currentPage->mouseDragged(args);
	zoomer->mouseDragged(args);
}

void ofxTimeline::mouseReleased(ofMouseEventArgs& args){
    if(modalTrack != NULL){
    	modalTrack->mouseReleased(args);
        return;
    }
    
    inoutTrack->mouseReleased(args);
	ticker->mouseReleased(args);
	tabs->mouseReleased(args);
	currentPage->mouseReleased(args);
	zoomer->mouseReleased(args);
}

void ofxTimeline::keyPressed(ofKeyEventArgs& args){

    if(modalTrack != NULL){
        modalTrack->keyPressed(args);
        return;
    }
    
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
	
    if(lockWidthToWindow){
        width = ofGetWidth();
    }
    
	if(pages.size() > 1){
		tabs->setDrawRect(ofRectangle(offset.x, offset.y, width, TICKER_HEIGHT));
	}
	else{
		tabs->setDrawRect(ofRectangle(offset.x, offset.y, width, 0));
	}
    
    ticker->setDrawRect( ofRectangle(offset.x, tabs->getBottomEdge(), width, showTicker ? TICKER_HEIGHT : 0) );
    inoutTrack->setDrawRect( ofRectangle(offset.x, ticker->getBottomEdge(), width, showInoutControl ? INOUT_HEIGHT : 0) );
    updatePagePositions();
	zoomer->setDrawRect(ofRectangle(offset.x, currentPage->getBottomEdge(), width, showZoomer ? ZOOMER_HEIGHT : 0));
    inoutTrack->setPageRectangle(currentPage->getDrawRect());
	ofRectangle tickerRect = ofRectangle(offset.x, ticker->getDrawRect().y,
                                        width, currentPage->getBottomEdge()-ticker->getDrawRect().y);
	ticker->setTotalDrawRect(tickerRect);		
	totalDrawRect = ofRectangle(offset.x, offset.y, width, zoomer->getDrawRect().y+zoomer->getDrawRect().height - offset.y);
}


void ofxTimeline::pageChanged(ofxTLPageEventArgs& args){
	for(int i = 0; i < pages.size(); i++){
		if(pages[i]->getName() == args.currentPageName){
			if(currentPage != NULL){
				currentPage->unselectAll();
			}
			currentPage = pages[i];
			ofEventArgs args;
			ofNotifyEvent(events().viewWasResized, args);
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

bool ofxTimeline::isDone(){
	return getPercentComplete() >= inoutRange.max && getLoopType() == OF_LOOP_NONE;   
}

void ofxTimeline::update(ofEventArgs& updateArgs){
	if(isFrameBased){
		currentTime = timecode.secondsForFrame(ofGetFrameNum() - playbackStartFrame);
    }
    else {
        currentTime = timer.getAppTime() - playbackStartTime;
    }
//		currentFrame = ofGetFrameNum() - playbackStartFr	ame;
//		if(currentFrame >= durationInFrames*inoutRange.max){
//			if(loopType == OF_LOOP_NONE){
//				currentFrame = durationInFrames*inoutRange.max;
//				stop();
//			}
//			else if(loopType == OF_LOOP_NORMAL && durationInFrames > 0) {
//				playbackStartFrame += getDurationInFrames() * inoutRange.span();
//				currentFrame %= int(durationInFrames * inoutRange.span());
//				currentFrame += durationInFrames*inoutRange.min;
//				ofxTLPlaybackEventArgs args = createPlaybackEvent();
//				ofNotifyEvent(ofxTLEvents.playbackLooped, args);
//			}
//		}
//	}
//	else{
//		currentTime = ofGetElapsedTimef() - playbackStartTime;
    
    if(currentTime >= durationInSeconds*inoutRange.max){
        if(loopType == OF_LOOP_NONE){
            currentTime = durationInSeconds*inoutRange.max;
            stop();
        }
        else if(loopType == OF_LOOP_NORMAL) {
            currentTime = durationInSeconds*inoutRange.min + fmod(currentTime, durationInSeconds*inoutRange.span());
            playbackStartFrame += getDurationInFrames()  * inoutRange.span();
            playbackStartTime  += getDurationInSeconds() * inoutRange.span();
            ofxTLPlaybackEventArgs args = createPlaybackEvent();
            ofNotifyEvent(events().playbackLooped, args);
        }
    }
}

void ofxTimeline::draw(){	
	if(isShowing){
		ofPushStyle();
		
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
        ofDisableLighting();
		ofEnableAlphaBlending();
		
		if (pages.size() > 1) {
			tabs->draw();			
		}
        
		currentPage->draw();
		if(showZoomer)zoomer->draw();
		if(showTicker)ticker->draw();
		if(showInoutControl)inoutTrack->draw();
        
		glPopAttrib();
		ofPopStyle();
	}
}

#pragma mark ELEMENT CREATORS/GETTERS/SETTERS
void ofxTimeline::addPage(string pageName, bool makeCurrent){
	if(pageName == ""){
		ofLogError("ofxTimeline -- Cannot add page with an empty name.");
		return;
	}
	
	for(int i = 0; i < pages.size(); i++){
		if(pageName == pages[i]->getName()){
			ofLogError("ofxTimeline -- Page " + pageName + " already exists");
			return;
		}
	}
    
	ofxTLPage* newPage = new ofxTLPage();
    newPage->timeline = this;
	newPage->setName(pageName);
	newPage->setup();
	newPage->setZoomBounds(zoomer->getViewRange());
	newPage->setTicker(ticker);
    
	pages.push_back(newPage);
	tabs->addPage(pageName);

	if(makeCurrent){
		tabs->selectPage(pageName);
	}
}

void ofxTimeline::setPageName(string newName){
	tabs->changeName(currentPage->getName(), newName);
	currentPage->setName( newName );
	currentPage->loadTrackPositions();
}

void ofxTimeline::setCurrentPage(string pageName){
	tabs->selectPage(pageName);
}

void ofxTimeline::setCurrentPage(int index){
	tabs->selectPage(index);
}

bool ofxTimeline::isModal(){
    return modalTrack != NULL;
}

ofxTLTrack* ofxTimeline::getModalTrack(){
    return modalTrack;
}

void ofxTimeline::setTimecontrolTrack(ofxTLTimeController* track){
    timeControl = track;
}

ofxTLTimeController* ofxTimeline::getTimecontrolTrack(){
    return timeControl;
}

//can be used to add custom elements
void ofxTimeline::addTrack(string trackName, ofxTLTrack* track){
	track->setTimeline( this );
	track->setName( trackName );
	currentPage->addTrack(trackName, track);		
	trackNameToPage[trackName] = currentPage;
	ofEventArgs args;
	ofNotifyEvent(events().viewWasResized, args);
}

ofxTLTweener* ofxTimeline::addKeyframes(string trackName, ofRange valueRange, float defaultValue){
    return addKeyframes(trackName, nameToXMLName(trackName), valueRange, defaultValue);
}

ofxTLTweener* ofxTimeline::addKeyframes(string trackName, string xmlFileName, ofRange valueRange, float defaultValue){

	ofxTLTweener* newKeyframer = new ofxTLTweener();
	newKeyframer->setCreatedByTimeline(true);
	newKeyframer->setValueRange(valueRange, defaultValue);
	newKeyframer->setXMLFileName(xmlFileName);
	addTrack(trackName, newKeyframer);
	
	return newKeyframer;
}

float ofxTimeline::getKeyframeValue(string trackName, float atTime){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find track " + trackName);
		return 0.0;
	}
	ofxTLTweener* keyframer = (ofxTLTweener*)trackNameToPage[trackName]->getTrack(trackName);
	if(keyframer == NULL){
		ofLogError("ofxTimeline -- Couldn't find track " + trackName);
		return 0.0;
	}
	return keyframer->getValueAtPercent(atTime/durationInSeconds);
}

float ofxTimeline::getKeyframeValue(string trackName){
	return getKeyframeValue(trackName, currentTime);    
}

float ofxTimeline::getKeyframeValue(string trackName, int atFrame){
    return getKeyframeValue(trackName, timecode.secondsForFrame(atFrame));
}

ofxTLTrack* ofxTimeline::getTrack(string trackName){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find track " + trackName);
		return NULL;
	}
	return trackNameToPage[trackName]->getTrack(trackName);
}

ofxTLSwitcher* ofxTimeline::addSwitcher(string trackName){
	return addSwitcher(trackName, nameToXMLName(trackName));
}

ofxTLSwitcher* ofxTimeline::addSwitcher(string trackName, string xmlFileName){
	ofxTLSwitcher* newSwitcher = new ofxTLSwitcher();
	newSwitcher->setCreatedByTimeline(true);
	newSwitcher->setXMLFileName(xmlFileName);
	addTrack(trackName, newSwitcher);
	return newSwitcher;
}

bool ofxTimeline::getSwitcherOn(string trackName, float atTime){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find switcher track " + trackName);
		return false;
	}
	
	ofxTLSwitcher* switcher = (ofxTLSwitcher*)trackNameToPage[trackName]->getTrack(trackName);
	if(switcher == NULL){
		ofLogError("ofxTimeline -- Couldn't find switcher track " + trackName);
		return false;
	}
    return switcher->isOn(atTime/durationInSeconds);   

}

bool ofxTimeline::getSwitcherOn(string trackName){
    return getSwitcherOn(trackName, currentTime);    
}

bool ofxTimeline::getSwitcherOn(string trackName, int atFrame){
	return getSwitcherOn(trackName, timecode.secondsForFrame(atFrame));	
}

ofxTLBangs* ofxTimeline::addBangs(string trackName){
 	return addBangs(trackName, nameToXMLName(trackName));   
}

ofxTLBangs* ofxTimeline::addBangs(string trackName, string xmlFileName){
	ofxTLBangs* newBangs = new ofxTLBangs();
	newBangs->setCreatedByTimeline(true);
	newBangs->setXMLFileName(xmlFileName);
	addTrack(trackName, newBangs);
	return newBangs;
}

ofxTLFlags* ofxTimeline::addFlags(string trackName){
    return addFlags(trackName, nameToXMLName(trackName));
}

ofxTLFlags* ofxTimeline::addFlags(string trackName, string xmlFileName){
    ofxTLFlags* newFlags = new ofxTLFlags();
	newFlags->setCreatedByTimeline(true);
	newFlags->setXMLFileName(xmlFileName);
	addTrack(trackName, newFlags);
	return newFlags;
}

ofxTLEvents& ofxTimeline::events(){
    return timelineEvents;
}

ofxTimecode& ofxTimeline::getTimecode(){
    return timecode;
}

ofxTLImageSequence* ofxTimeline::addImageSequence(string trackName){
	ofFileDialogResult result = ofSystemLoadDialog("Load Sequence", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		return addImageSequence(trackName, result.getPath());
	}
	return NULL;
}

ofxTLImageSequence* ofxTimeline::addImageSequence(string trackName, string directory){
	ofxTLImageSequence*	newImageSequence = new ofxTLImageSequence();
	newImageSequence->setCreatedByTimeline(true);
	newImageSequence->loadSequence(directory);
	addTrack(trackName, newImageSequence);
	return newImageSequence;	
}

//TODO:
ofImage* ofxTimeline::getImage(string trackName){
	return NULL;
}

ofImage* ofxTimeline::getImage(string trackName, float atTime){
	return NULL;
}

ofImage* ofxTimeline::getImage(string trackName, int atFrame){
	return NULL;
}

ofxTLVideoTrack* ofxTimeline::addVideoTrack(string trackName, string videoPath){
	ofxTLVideoTrack* videoTrack = new ofxTLVideoTrack();
    if(videoTrack->load(videoPath)){
        videoTrack->setCreatedByTimeline(true);
        addTrack(trackName, videoTrack);
        return videoTrack;
    }
    else {
        ofLogError("ofxTimeline::addVideoTrack -- video " + videoPath + " failed to load");
    }
    delete videoTrack;
    return NULL;
}

ofxTLVideoTrack* ofxTimeline::getVideoTrack(string videoTrackName){
	return (ofxTLVideoTrack*)getTrack(videoTrackName);
}

ofPtr<ofVideoPlayer> ofxTimeline::getVideoPlayer(string videoTrackName){
    ofxTLVideoTrack* track = getVideoTrack(videoTrackName);
    if(track == NULL){
        ofLogError("ofxTimeline::getPlayer -- video player is null");
        return ofPtr<ofVideoPlayer>(); //null ptr
    }
    return track->getPlayer();
}

void ofxTimeline::bringTrackToTop(string trackName){
    bringTrackToTop(getTrack(trackName));
}

void ofxTimeline::bringTrackToTop(ofxTLTrack* track){
    if(track != NULL){
		trackNameToPage[track->getName()]->bringTrackToTop(track);
    }
}

void ofxTimeline::bringTrackToBottom(string trackName){
	bringTrackToBottom(getTrack(trackName));    
}

void ofxTimeline::bringTrackToBottom(ofxTLTrack* track){
    if(track != NULL){
		trackNameToPage[track->getName()]->bringTrackToBottom(track);        
    }
}

//TODO: replace with ofxTimecode formatting
string ofxTimeline::formatTime(float time){
    return timecode.timecodeForSeconds(time);
}

string ofxTimeline::nameToXMLName(string trackName){
    string xmlName = name + "_" + trackName;
    ofStringReplace(xmlName, " ", "_");
    ofStringReplace(xmlName, ":", "_");
    ofStringReplace(xmlName, "/", "_");
    ofStringReplace(xmlName, "\\", "_");
	xmlName += ".xml";
	return xmlName;	    
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

float ofxTimeline::screenXtoNormalizedX(float x){
	//return ofMap(x, bounds.x, bounds.x+bounds.width, zoomBounds.min, zoomBounds.max, true);
    return screenXtoNormalizedX(x, zoomer->getViewRange());
}

float ofxTimeline::normalizedXtoScreenX(float x){
    //	return ofMap(x, zoomBounds.min, zoomBounds, bounds.x, bounds.x+bounds.width, true);
    return normalizedXtoScreenX(x, zoomer->getViewRange());
}

float ofxTimeline::screenXtoNormalizedX(float x, ofRange outputRange){
	return ofMap(x, getDrawRect().x, getDrawRect().x+getDrawRect().width, outputRange.min, outputRange.max, false);
}

float ofxTimeline::normalizedXtoScreenX(float x, ofRange inputRange){
	return ofMap(x, inputRange.min, inputRange.max, getDrawRect().x, getDrawRect().x+getDrawRect().width, false);
}
