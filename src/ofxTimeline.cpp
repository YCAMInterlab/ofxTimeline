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
//#include "ofxTLUtils.h"
#include "ofxHotKeys.h"

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
	snapToBPM(false),
	snapToOtherElements(true),
	dragMillsecondOffset(0),
	movePlayheadOnPaste(true),
	movePlayheadOnDrag(false),
	inoutRange(ofRange(0.0,1.0)),
	currentPage(NULL),
	modalTrack(NULL),
	timeControl(NULL),
	loopType(OF_LOOP_NONE),
	lockWidthToWindow(true),
	currentTime(0.0),
	undoPointer(0),
	undoEnabled(true),
	isOnThread(false),
	defaultPalettePath("defaultColorPalette.png") //copy from ofxTimeline/assets into bin/data/
{
}

ofxTimeline::~ofxTimeline(){
	if(isSetup){
		ofRemoveListener(timelineEvents.viewWasResized, this, &ofxTimeline::viewWasResized);
		ofRemoveListener(timelineEvents.pageChanged, this, &ofxTimeline::pageChanged);

        //TODO: move to shared pointers 
        //this breaks timelines that are statically declared because 
        //there is no copy/assignment constructor
		reset();
        
        delete ticker;
        delete tabs;
        delete zoomer;
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
	
	colors.load();

	enable();
    
	ofAddListener(timelineEvents.viewWasResized, this, &ofxTimeline::viewWasResized);
	ofAddListener(timelineEvents.pageChanged, this, &ofxTimeline::pageChanged);

    //You can change this name by calling setPageName()
	addPage("Page One", true);
    
    setName("timeline" + ofToString(timelineNumber++));

}

void ofxTimeline::moveToThread(){
	if(!isOnThread){
		isOnThread = true;
		stop();
		startThread();
		
	}
}


void ofxTimeline::setName(string newName){
    if(newName != name){
        
	    name = newName;
		
        inoutTrack->setXMLFileName( ofToDataPath(workingFolder + name + "_inout.xml") );
        inoutTrack->setup();
        
        zoomer->setXMLFileName( ofToDataPath(workingFolder + name + "_zoomer.xml") );
        zoomer->setup();
        
        currentPage->loadTrackPositions();
    }
}

string ofxTimeline::getName(){
    return name;
}

void ofxTimeline::setWorkingFolder(string folderPath){
	workingFolder = folderPath = ofFilePath::addTrailingSlash(folderPath);
    inoutTrack->setXMLFileName( ofToDataPath(workingFolder + name + "_inout.xml") );
    zoomer->setXMLFileName( ofToDataPath(workingFolder + name + "_zoomer.xml") );
}

string ofxTimeline::getWorkingFolder(){
    return workingFolder;
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
    setShowInoutControl(shouldShowTimeControls);
    setShowZoomer(shouldShowTimeControls);
    setShowTicker(shouldShowTimeControls);
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
    if(showZoomer){
		zoomer->load();        
    }
    else{
        zoomer->setViewRange(ofRange(0,1.0));
    }
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

//turn on undo
void ofxTimeline::enableUndo(bool enabled){
	undoEnabled = enabled;    
}

void ofxTimeline::undo(){
    if(undoPointer > 0){
    	undoPointer--;
        restoreToState(undoStack[undoPointer]);
    }
}

void ofxTimeline::redo(){
    if(undoPointer < undoStack.size()-1){
        undoPointer++;
		restoreToState(undoStack[undoPointer]);
    }
}

void ofxTimeline::restoreToState(vector<UndoItem>& state){
    for(int i = 0; i < state.size(); i++){
//		cout << "restoring state for track " << state[i].track->getDisplayName() << endl;
//		cout << state[i].stateBuffer << endl;
        state[i].track->loadFromXMLRepresentation(state[i].stateBuffer);
    }
}

//called on mouse down and key down
//stores the state of all tracks that could potentially be modified
//by this action so that we can push ones that actually were changed
//onto the undo stack
void ofxTimeline::collectStateBuffers(){
    
    if(!undoEnabled) return;
    
    vector<ofxTLTrack*> tracks = currentPage->getTracks();
    stateBuffers.clear();
    modifiedTracks.clear();
    for(int i = 0; i < tracks.size(); i++){
        ofxTLTrack* track = tracks[i];
        if(track->getSelectedItemCount() > 0 || track->isActive() ){

            UndoItem ui;
            ui.track = track;
            ui.stateBuffer = track->getXMLRepresentation();
            stateBuffers.push_back(ui);
//			cout << "collecting state for " << track->getDisplayName() << endl;
//			cout << ui.stateBuffer << endl;
	
        }
    }
}

//go through the state buffers and see which tracks were actually modified
//push the collection of them onto the stack if there were any
void ofxTimeline::pushUndoStack(){
    
    if(!undoEnabled) return;
    
    vector<UndoItem> undoCollection;
    for(int i = 0; i < modifiedTracks.size(); i++){
        for(int buf = 0; buf < stateBuffers.size(); buf++){
            //this m
            if(modifiedTracks[i] == stateBuffers[buf].track){
                undoCollection.push_back(stateBuffers[buf]);
            }
        }
    }
    if(undoCollection.size() > 0){
        //remove any history that we've undone
        while(undoPointer < undoStack.size()){
            undoStack.pop_back();
        }
        undoStack.push_back(undoCollection);
        undoPointer = undoStack.size();
        
        //store the most recent state at the top of the queue
        vector<UndoItem> currentState;
        for(int i = 0; i < modifiedTracks.size(); i++){
            UndoItem ui;
            ui.track = modifiedTracks[i];
            ui.stateBuffer = modifiedTracks[i]->getXMLRepresentation();
            currentState.push_back(ui);
        }
        undoStack.push_back(currentState);
    }
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
//	cout << "modified track " << track->getDisplayName() << endl;
	flagUserChangedValue();
    
    if(undoEnabled){
        modifiedTracks.push_back(track);
    }
    
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
        currentTime = ofClamp(currentTime, getInTimeInSeconds(), getOutTimeInSeconds());
        
        playbackStartTime = timer.getAppTimeSeconds() - currentTime;
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
        return false;
    }
    
    if(timeControl != NULL){
        return timeControl->togglePlay();
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

void ofxTimeline::setCurrentTimecode(string timecodeString){
    currentTime = timecode.secondsForTimecode(timecodeString);
}

void ofxTimeline::setHoverTime(long millisTime){
	ticker->setHoverTime(millisTime);
}

void ofxTimeline::setCurrentTimeSeconds(float time){
	currentTime = time;
}

void ofxTimeline::setCurrentTimeMillis(long millis){
	currentTime = millis/1000.;
}

void ofxTimeline::setFrameRate(float fps){
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

long ofxTimeline::getCurrentTimeMillis(){
    return currentTime*1000;
}

float ofxTimeline::getCurrentTime(){
	return currentTime;
}

float ofxTimeline::getPercentComplete(){
    return currentTime / durationInSeconds;
}

string ofxTimeline::getCurrentTimecode(){
    return timecode.timecodeForSeconds(currentTime);
}

void ofxTimeline::setInPointAtPlayhead(){
    setInPointAtSeconds(currentTime);
}
void ofxTimeline::setInPointAtPercent(float percent){
	inoutRange.min = ofClamp(percent, 0, inoutRange.max);
}
void ofxTimeline::setInPointAtSeconds(float time){
	setInPointAtPercent(time/durationInSeconds);	    
}
void ofxTimeline::setInPointAtFrame(int frame){
    setInPointAtPercent(timecode.secondsForFrame(frame) / durationInSeconds);
}
void ofxTimeline::setInPointAtMillis(long millis){
    setInPointAtPercent(millis / (1000. * durationInSeconds) );
}
void ofxTimeline::setInPointAtTimecode(string timecodeString){
    setInPointAtPercent(timecode.secondsForTimecode(timecodeString) / durationInSeconds);
}

void ofxTimeline::setOutPointAtPlayhead(){
    setOutPointAtSeconds(currentTime);
}
void ofxTimeline::setOutPointAtPercent(float percent){
	inoutRange.max = ofClamp(percent, inoutRange.min, 1.0);
}
void ofxTimeline::setOutPointAtFrame(float frame){
    setOutPointAtPercent(timecode.secondsForFrame(frame) / durationInSeconds);
}
void ofxTimeline::setOutPointAtSeconds(float time){
    setOutPointAtPercent(time/durationInSeconds);
}
void ofxTimeline::setOutPointAtMillis(long millis){
    setOutPointAtPercent(millis / (1000. * durationInSeconds) );
}
void ofxTimeline::setOutPointAtTimecode(string timecodeString){
    setOutPointAtPercent(timecode.secondsForTimecode(timecodeString) / durationInSeconds);    
}

void ofxTimeline::setInOutRange(ofRange inoutPercentRange){
    if(inoutPercentRange.min > inoutPercentRange.max) return;
	inoutRange = inoutPercentRange;
}

void ofxTimeline::setCurrentTimeToInPoint(){
    setPercentComplete(inoutRange.min);
}

void ofxTimeline::setCurrentTimeToOutPoint(){
    setPercentComplete(inoutRange.max);
}

void ofxTimeline::clearInOut(){
	setInOutRange(ofRange(0.0,1.0));    
}

ofRange ofxTimeline::getInOutRange(){
	return inoutRange;
}

int ofxTimeline::getInFrame(){
	return timecode.frameForSeconds(getInTimeInSeconds());
}

int ofxTimeline::getOutFrame(){
    return timecode.frameForSeconds(getOutTimeInSeconds());
}

float ofxTimeline::getInTimeInSeconds(){
	return durationInSeconds*inoutRange.min;
}

float ofxTimeline::getOutTimeInSeconds(){
	return durationInSeconds*inoutRange.max;
}

long ofxTimeline::getInTimeInMillis(){
    return getInTimeInSeconds()*1000;
}

string ofxTimeline::getInPointTimecode(){
	return timecode.timecodeForSeconds(getInTimeInSeconds());
}

long ofxTimeline::getOutTimeInMillis(){
    return getOutTimeInSeconds()*1000;    
}

string ofxTimeline::getOutPointTimecode(){
	return timecode.timecodeForSeconds(getOutTimeInSeconds());    
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
        stop();
		isEnabled = false;
		disableEvents();
    }
}

//clears every element
//TODO how should this work with Undo??
void ofxTimeline::clear(){
	for(int i = 0; i < pages.size(); i++){
        pages[i]->clear();
    }
}

void ofxTimeline::reset(){ //gets rid of everything
    stop();
    undoStack.clear();
    for(int i = 0; i < pages.size(); i++){ 
        delete pages[i];
    }
	
	tabs->clear();
    setInOutRange(ofRange(0,1.0));
    pages.clear();
    trackNameToPage.clear();
    currentPage = NULL;
    modalTrack = NULL;
    timeControl = NULL;
	addPage("Page One", true);

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
    	ofLogError("ofxTimeline::setDurationInSeconds") << " must set a positive duration";
        return;
    }
	durationInSeconds = seconds;
}

void ofxTimeline::setDurationInMillis(long millis){
    durationInSeconds = millis/1000.;
}

void ofxTimeline::setDurationInTimecode(string timecodeString){
    float newDuration = timecode.secondsForTimecode(timecodeString);
    if(newDuration > 0){
	    durationInSeconds = newDuration;
    }
    else{
        ofLogError() << "ofxTimeline::setDurationInTimecode -- " << timecodeString << " is invalid, please use the format HH:MM:SS:MLS";
    }
}

int ofxTimeline::getDurationInFrames(){
    return timecode.frameForSeconds(durationInSeconds);
}

long ofxTimeline::getDurationInMilliseconds(){
	return durationInSeconds*1000;    //TODO: update to store currentTime in millis
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
    if(offset != newOffset){
        offset = newOffset;
        updatePagePositions();
        ofEventArgs args;
        ofNotifyEvent(timelineEvents.viewWasResized, args);
    }
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
    if(width != newWidth){
        width = newWidth;
        updatePagePositions();
        ofEventArgs args;
        ofNotifyEvent(events().viewWasResized, args);
    }
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
		if(currentPage != NULL){
			currentPage->recalculateHeight();
		}
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

void ofxTimeline::setBPM(float bpm) {
    ticker->setBPM(bpm);
}

float ofxTimeline::getBPM() {
	return ticker->getBPM();
}

bool ofxTimeline::toggleSnapToBPM(){
	snapToBPM = !snapToBPM;
    return snapToBPM;
}

void ofxTimeline::enableSnapToBPM(bool enableSnap){
    snapToBPM = enableSnap;
}

bool ofxTimeline::getSnapToBPM(){
    return snapToBPM;
}

bool ofxTimeline::toggleShowBPMGrid(){
    ticker->setDrawBPMGrid(!ticker->getDrawBPMGrid());		
	return ticker->getDrawBPMGrid();
}

void ofxTimeline::setShowBPMGrid(bool enableGrid){
    ticker->setDrawBPMGrid(enableGrid);
}

bool ofxTimeline::getShowBPMGrid(){
    return ticker->getDrawBPMGrid();
}

bool ofxTimeline::toggleSnapToOtherKeyframes(){
    snapToOtherElements = !snapToOtherElements;
    return snapToOtherElements;
}

void ofxTimeline::enableSnapToOtherKeyframes(bool enableSnapToOther){
    snapToOtherElements = enableSnapToOther;
}

bool ofxTimeline::getSnapToOtherElements(){
    return snapToOtherElements;
}

#pragma mark EVENTS
void ofxTimeline::enableEvents() {
	if (!usingEvents) {
		ofAddListener(ofEvents().mouseMoved, this, &ofxTimeline::mouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &ofxTimeline::mousePressed);
		ofAddListener(ofEvents().mouseReleased, this, &ofxTimeline::mouseReleased);
		ofAddListener(ofEvents().mouseDragged, this, &ofxTimeline::mouseDragged);
//		ofAddListener(ofEvents().draw, this, &ofxTimeline::draw);

		ofAddListener(ofEvents().keyPressed, this, &ofxTimeline::keyPressed);
		ofAddListener(ofEvents().keyReleased, this, &ofxTimeline::keyReleased);
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
		ofRemoveListener(ofEvents().keyReleased, this, &ofxTimeline::keyReleased);
		ofRemoveListener(ofEvents().windowResized, this, &ofxTimeline::windowResized);
		
		usingEvents = false;
	}
}

void ofxTimeline::mousePressed(ofMouseEventArgs& args){
    long millis = screenXToMillis(args.x);
    
    if(modalTrack != NULL){
    	modalTrack->mousePressed(args,millis);
    }
    else{
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
		currentPage->mousePressed(args,millis);
		zoomer->mousePressed(args);
		currentPage->setSnappingEnabled((snapToBPM || snapToOtherElements) && dragAnchorSet);
	}
	
    //collect state buffers after items are selected and focus is set
    collectStateBuffers();

}

void ofxTimeline::mouseMoved(ofMouseEventArgs& args){
    long millis = screenXToMillis(args.x);
    
    if(modalTrack != NULL){
    	modalTrack->mouseMoved(args, millis);
        return;
    }
    
    inoutTrack->mouseMoved(args);
	ticker->mouseMoved(args);
	currentPage->mouseMoved(args, millis);
	zoomer->mouseMoved(args);
}

void ofxTimeline::mouseDragged(ofMouseEventArgs& args){
    long millis = screenXToMillis(args.x);
    
    if(modalTrack != NULL){
    	modalTrack->mouseDragged(args, false);
        return;
    }
    
	inoutTrack->mouseDragged(args);
	ticker->mouseDragged(args);
	currentPage->mouseDragged(args, millis);
	zoomer->mouseDragged(args);
}

void ofxTimeline::mouseReleased(ofMouseEventArgs& args){
    long millis = screenXToMillis(args.x);
    
    dragAnchorSet = false;

    if(modalTrack != NULL){
    	modalTrack->mouseReleased(args, millis);
	}
    else{
		inoutTrack->mouseReleased(args);
		ticker->mouseReleased(args);
		tabs->mouseReleased(args);
		currentPage->mouseReleased(args, millis);
		zoomer->mouseReleased(args);
	}	
    
    pushUndoStack();
}

void ofxTimeline::keyPressed(ofKeyEventArgs& args){
	
	if(ofGetModifierShortcutKeyPressed() && ofGetModifierShiftPressed() && args.key == 'z' && undoEnabled){
        redo();
		return;
    }
    else if(ofGetModifierShortcutKeyPressed() && args.key == 'z' && undoEnabled){
        undo();
		return;
    }

	//collect the buffers before the command is sent becasue it's what modifies
    collectStateBuffers();

    
    if(modalTrack != NULL){
        modalTrack->keyPressed(args);
        return;
    }
    
//    cout << "key event " << args.key << " ctrl? " << ofGetModifierKeyControl() << " " << ofGetModifierKeyShift() << endl;
    
	if(ofGetModifierShortcutKeyPressed() && args.key == 'c'){ //copy
		string copyattempt = currentPage->copyRequest();
		if(copyattempt != ""){
			pasteboard = copyattempt;
		}
	}
	else if(ofGetModifierShortcutKeyPressed() && args.key == 'x'){ //cut
		string copyattempt = currentPage->cutRequest();
		if(copyattempt != ""){
			pasteboard = copyattempt;
		}
	}
	else if(ofGetModifierShortcutKeyPressed() && args.key == 'v'){ //paste
		if (pasteboard != "") {
			currentPage->pasteSent(pasteboard);
		}				
	}
	else if(ofGetModifierShortcutKeyPressed() && args.key == 'a'){ //select all
		currentPage->selectAll();						
	}
	else{
		if(args.key >= OF_KEY_LEFT && args.key <= OF_KEY_DOWN){
			ofVec2f nudgeAmount = ofGetModifierShiftPressed() ? getBigNudgePercent() : getNudgePercent();

			if(getTotalSelectedItems() == 0){
				if(args.key == OF_KEY_LEFT){
					if(getIsFrameBased()){
						currentTime -= timecode.secondsForFrame(1);
					}
					else{
						currentTime -= nudgeAmount.x*getDurationInSeconds();
					}
				}
				if(args.key == OF_KEY_RIGHT){
					if(getIsFrameBased()){
						currentTime += timecode.secondsForFrame(1);
					}
					else{
						currentTime += nudgeAmount.x*getDurationInSeconds();
					}
				}
			}
			else{
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
		}
		
		ticker->keyPressed(args);
		currentPage->keyPressed(args);
		zoomer->keyPressed(args);
	}
}

void ofxTimeline::keyReleased(ofKeyEventArgs& args){
    pushUndoStack();
}

void ofxTimeline::windowResized(ofResizeEventArgs& args){
    ofNotifyEvent(events().viewWasResized, args, this);
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
        currentTime = timer.getAppTimeSeconds() - playbackStartTime;
    }
	
    checkLoop();
	checkEvents();
}

void ofxTimeline::threadedFunction(){
	while(isThreadRunning()){
		if(isPlaying){
			currentTime = timer.getAppTimeSeconds() - playbackStartTime;
			
			checkLoop();
			checkEvents();
		}
		ofSleepMillis(1);
	}
}

void ofxTimeline::checkEvents(){
	for(int i = 0; i < pages.size(); i++){
		pages[i]->update();
	}
}

void ofxTimeline::checkLoop(){
	if(currentTime < durationInSeconds*inoutRange.min){
		//        cout << "BELOW MIN " << endl;
        currentTime = durationInSeconds*inoutRange.min;
        playbackStartTime = timer.getAppTimeSeconds() - currentTime;
        playbackStartFrame = ofGetFrameNum() - timecode.frameForSeconds(currentTime);
    }
    
    if(currentTime >= durationInSeconds*inoutRange.max){
        if(loopType == OF_LOOP_NONE){
            currentTime = durationInSeconds*inoutRange.max;
            stop();
        }
        else if(loopType == OF_LOOP_NORMAL) {
            currentTime = durationInSeconds*inoutRange.min + (currentTime - durationInSeconds*inoutRange.max);
            playbackStartFrame += getDurationInFrames()  * inoutRange.span();
            playbackStartTime  += getDurationInSeconds() * inoutRange.span();
            ofxTLPlaybackEventArgs args = createPlaybackEvent();
            ofNotifyEvent(events().playbackLooped, args);
        }
    }
}

//void ofxTimeline::draw(ofEventArgs& args){
void ofxTimeline::draw(){
	if(isShowing){
		ofPushStyle();
//		cout << "timeline draw" << endl;
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
        ofDisableLighting();
		ofEnableAlphaBlending();
		
		if (pages.size() > 1) {
			tabs->draw();			
		}
        
		currentPage->draw();
		if(showZoomer)zoomer->draw();
        
		//draw these because they overlay the rest of the timeline with info
        ticker->_draw();
		inoutTrack->_draw();
        
		if(modalTrack != NULL){
			modalTrack->drawModalContent();
		}
			
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

void ofxTimeline::setPageName(string newName, int index){
	if(index < 0 || index >= pages.size()){
        ofLogError("ofxTimeline::setPageName -- index out of bounds");
        return;
    }
    tabs->changeName(pages[index]->getName(), newName);
    pages[index]->setName(newName);
    pages[index]->loadTrackPositions();
}

void ofxTimeline::setCurrentPage(string pageName){
	tabs->selectPage(pageName);
}

void ofxTimeline::setCurrentPage(int index){
	tabs->selectPage(index);
}

int ofxTimeline::getTotalSelectedItems(){
    int totalSelected = 0;
    vector<ofxTLTrack*> tracks = currentPage->getTracks();
    for(int i = 0; i < tracks.size(); i++){
        totalSelected += tracks[i]->getSelectedItemCount();
    }
    return totalSelected;
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
	if(trackNameToPage[trackName] != NULL){
        ofLogError() << "ofxTimeline::addTrack -- Overriding track " << trackName;
    }
	track->setTimeline( this );
	track->setName( trackName );
	currentPage->addTrack(trackName, track);	
	trackNameToPage[trackName] = currentPage;
	ofEventArgs args;
	ofNotifyEvent(events().viewWasResized, args);
}

ofxTLCurves* ofxTimeline::addCurves(string trackName, ofRange valueRange, float defaultValue){
    string uniqueName = confirmedUniqueName(trackName);
    return addCurves(uniqueName, nameToXMLName(uniqueName), valueRange, defaultValue);
}

ofxTLCurves* ofxTimeline::addCurves(string trackName, string xmlFileName, ofRange valueRange, float defaultValue){
	ofxTLCurves* newCurves = new ofxTLCurves();
	newCurves->setCreatedByTimeline(true);
	newCurves->setValueRange(valueRange, defaultValue);
	newCurves->setXMLFileName(xmlFileName);
	addTrack(confirmedUniqueName(trackName), newCurves);
	return newCurves;
}

float ofxTimeline::getValue(string trackName, float atTime){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find track " + trackName);
		return 0.0;
	}
	ofxTLCurves* keyframer = (ofxTLCurves*)trackNameToPage[trackName]->getTrack(trackName);
	if(keyframer == NULL){
		ofLogError("ofxTimeline -- Couldn't find track " + trackName);
		return 0.0;
	}
	return keyframer->getValueAtPercent(atTime/durationInSeconds);
}

float ofxTimeline::getValue(string trackName){
	return getValue(trackName, currentTime);    
}

float ofxTimeline::getValue(string trackName, int atFrame){
    return getValue(trackName, timecode.secondsForFrame(atFrame));
}

ofxTLTrack* ofxTimeline::getTrack(string trackName){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find track " + trackName);
		return NULL;
	}
	return trackNameToPage[trackName]->getTrack(trackName);
}

ofxTLSwitches* ofxTimeline::addSwitches(string trackName){
    string uniqueName = confirmedUniqueName(trackName);
	return addSwitches(uniqueName, nameToXMLName(uniqueName));
}

ofxTLSwitches* ofxTimeline::addSwitches(string trackName, string xmlFileName){
	ofxTLSwitches* newSwitches = new ofxTLSwitches();
	newSwitches->setCreatedByTimeline(true);
	newSwitches->setXMLFileName(xmlFileName);
	addTrack(confirmedUniqueName(trackName), newSwitches);
	return newSwitches;
}

bool ofxTimeline::isSwitchOn(string trackName, float atTime){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find switcher track " + trackName);
		return false;
	}
	
	ofxTLSwitches* switches = (ofxTLSwitches*)trackNameToPage[trackName]->getTrack(trackName);
	if(switches == NULL){
		ofLogError("ofxTimeline -- Couldn't find switcher track " + trackName);
		return false;
	}
    return switches->isOnAtPercent(atTime/durationInSeconds);
}

bool ofxTimeline::isSwitchOn(string trackName){
    return isSwitchOn(trackName, currentTime);    
}

bool ofxTimeline::isSwitchOn(string trackName, int atFrame){
	return isSwitchOn(trackName, timecode.secondsForFrame(atFrame));	
}

ofxTLBangs* ofxTimeline::addBangs(string trackName){
    string uniqueName = confirmedUniqueName(trackName);
 	return addBangs(uniqueName, nameToXMLName(uniqueName));   
}

ofxTLBangs* ofxTimeline::addBangs(string trackName, string xmlFileName){
	ofxTLBangs* newBangs = new ofxTLBangs();
	newBangs->setCreatedByTimeline(true);
	newBangs->setXMLFileName(xmlFileName);
	addTrack(confirmedUniqueName(trackName), newBangs);
	return newBangs;
}

ofxTLFlags* ofxTimeline::addFlags(string trackName){
    string uniqueName = confirmedUniqueName(trackName);
    return addFlags(uniqueName, nameToXMLName(uniqueName));
}

ofxTLFlags* ofxTimeline::addFlags(string trackName, string xmlFileName){
    ofxTLFlags* newFlags = new ofxTLFlags();
	newFlags->setCreatedByTimeline(true);
	newFlags->setXMLFileName(xmlFileName);
	addTrack(confirmedUniqueName(trackName), newFlags);
	return newFlags;
}

ofxTLColorTrack* ofxTimeline::addColors(string trackName){
	string uniqueName = confirmedUniqueName(trackName);
	return addColorsWithPalette(uniqueName, nameToXMLName(uniqueName), defaultPalettePath);
}

ofxTLColorTrack* ofxTimeline::addColors(string name, string xmlFileName){
	return addColorsWithPalette(confirmedUniqueName(name), xmlFileName, defaultPalettePath);
}

ofxTLColorTrack* ofxTimeline::addColorsWithPalette(string trackName, ofImage& palette){
	string uniqueName = confirmedUniqueName(trackName);
	return addColorsWithPalette(uniqueName, nameToXMLName(uniqueName), palette);
}

ofxTLColorTrack* ofxTimeline::addColorsWithPalette(string trackName, string palettePath){
	string uniqueName = confirmedUniqueName(trackName);
	return addColorsWithPalette(uniqueName, nameToXMLName(uniqueName), palettePath);
}

ofxTLColorTrack* ofxTimeline::addColorsWithPalette(string trackName, string xmlFileName, ofImage& palette){
	ofxTLColorTrack* newColors = new ofxTLColorTrack();
	newColors->setCreatedByTimeline(true);
	newColors->setXMLFileName(xmlFileName);
	newColors->loadColorPalette(palette);
	addTrack(confirmedUniqueName(trackName), newColors);
	return newColors;
}

ofxTLColorTrack* ofxTimeline::addColorsWithPalette(string trackName, string xmlFileName, string palettePath){
	ofxTLColorTrack* newColors = new ofxTLColorTrack();
	newColors->setCreatedByTimeline(true);
	newColors->setXMLFileName(xmlFileName);
	newColors->loadColorPalette(palettePath);
	addTrack(confirmedUniqueName(trackName), newColors);
	return newColors;
}

ofColor ofxTimeline::getColor(string trackName){
	return getColorAtMillis(trackName, getCurrentTimeMillis());
}

ofColor ofxTimeline::getColorAtPercent(string trackName, float percent){
	return getColorAtMillis(trackName, percent*getDurationInMilliseconds());
}

ofColor ofxTimeline::getColorAtSecond(string trackName, float second){
	return getColorAtMillis(trackName, second*1000);
}

ofColor ofxTimeline::getColorAtMillis(string trackName, unsigned long millis){
	if(trackNameToPage.find(trackName) == trackNameToPage.end()){
		ofLogError("ofxTimeline -- Couldn't find color track " + trackName);
		return ofColor(0,0,0);
	}
	
	ofxTLColorTrack* colors = (ofxTLColorTrack*)trackNameToPage[trackName]->getTrack(trackName);
	if(colors == NULL){
		ofLogError("ofxTimeline -- Couldn't find switcher track " + trackName);
		return ofColor(0,0,0);
	}
	return colors->getColorAtMillis(millis);
}

string ofxTimeline::getDefaultColorPalettePath(){
	return defaultPalettePath;
}

//*** IMAGE SEQUENCE DOESN'T WORK **///
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
	addTrack(confirmedUniqueName(trackName), newImageSequence);
	return newImageSequence;	
}


ofImage* ofxTimeline::getImage(string trackName){
	return NULL;
}

ofImage* ofxTimeline::getImage(string trackName, float atTime){
	return NULL;
}

ofImage* ofxTimeline::getImage(string trackName, int atFrame){
	return NULL;
}

ofxTLVideoTrack* ofxTimeline::addVideoTrack(string trackName){
	return addVideoTrack(trackName, "");
}


ofxTLVideoTrack* ofxTimeline::addVideoTrack(string trackName, string videoPath){
	ofxTLVideoTrack* videoTrack = new ofxTLVideoTrack();
	videoTrack->setCreatedByTimeline(true);
	addTrack(confirmedUniqueName(trackName), videoTrack);
	if(videoPath != ""){
	    if(!videoTrack->load(videoPath)){
        	ofLogError("ofxTimeline::addVideoTrack -- video " + videoPath + " failed to load");
		}
    }
    return videoTrack;
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

ofxTLTrackHeader* ofxTimeline::getTrackHeader(string trackName){
    return getTrackHeader(getTrack(name));    
}

ofxTLTrackHeader* ofxTimeline::getTrackHeader(ofxTLTrack* track){
    return trackNameToPage[track->getName()]->getTrackHeader(track);
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

void ofxTimeline::removeTrack(string name){
    ofxTLTrack* track = getTrack(name);
    if(track == NULL){
        ofLogError() << "ofxTimeline::removeTrack -- Could not find track " << name << " to remove " << endl;
        return;
	}
    if(track == modalTrack){
        modalTrack = NULL;
    }

    //quick fix for now -- we need to have Undo and Delete track work together
    //but to prevent crashes, let's just go through the undo queue and remove any items that have to do with this track
    for(int i = 0; i < undoStack.size(); i++){
        for(int q = undoStack[i].size()-1; q >= 0; q--){
			if(undoStack[i][q].track == track){
                undoStack[i].erase(undoStack[i].begin() + q);
                cout << "temporary fix -- deleting undo queue element for track " << track->getName() << endl;
            }
        }
    }

    trackNameToPage[name]->removeTrack(track);
    trackNameToPage.erase(name);
	ofEventArgs args;
	ofNotifyEvent(events().viewWasResized, args);
}

void ofxTimeline::removeTrack(ofxTLTrack* track){
    removeTrack(track->getName()); //always delete by name to ensure the track is in this timeline
}

ofxTLEvents& ofxTimeline::events(){
    return timelineEvents;
}

ofxTimecode& ofxTimeline::getTimecode(){
    return timecode;
}

vector<ofxTLPage*>& ofxTimeline::getPages(){
    return pages;
}


string ofxTimeline::formatTime(float seconds){
    return timecode.timecodeForSeconds(seconds);
}

string ofxTimeline::formatTime(long millis){
    return timecode.timecodeForMillis(millis);
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

string ofxTimeline::confirmedUniqueName(string name){
    string uniqueName = name;
    int attempts = 0;
    while(trackNameToPage[uniqueName] != NULL){
        uniqueName = name + "_" + ofToString(attempts++);
    }
    return uniqueName;
}

void ofxTimeline::setDragTimeOffset(long millisecondOffset){

	dragMillsecondOffset = millisecondOffset;

    currentPage->setDragOffsetTime(dragMillsecondOffset);
	dragAnchorSet = true;
}

//this can be called from track classes and will cancel snapping
void ofxTimeline::cancelSnapping(){
	dragAnchorSet = false;
}

long ofxTimeline::getDragTimeOffset(){
	return dragAnchorSet ? dragMillsecondOffset : 0.;
}

ofVec2f ofxTimeline::getNudgePercent(){
	return ofVec2f(zoomer->getViewRange().span()*.001, .001);
}

ofVec2f ofxTimeline::getBigNudgePercent(){
	return ofVec2f(zoomer->getViewRange().span()*.02, 0.02);	
}

long ofxTimeline::screenXToMillis(float x){
	return screenXtoNormalizedX(x) * durationInSeconds * 1000;
}

float ofxTimeline::millisToScreenX(long millis){
    return normalizedXtoScreenX(millis/(durationInSeconds*1000));
}

float ofxTimeline::screenXtoNormalizedX(float x){
    return screenXtoNormalizedX(x, zoomer->getViewRange());
}

float ofxTimeline::normalizedXtoScreenX(float x){
    return normalizedXtoScreenX(x, zoomer->getViewRange());
}

float ofxTimeline::screenXtoNormalizedX(float x, ofRange outputRange){
	return ofMap(x, getDrawRect().x, getDrawRect().x+getDrawRect().width, outputRange.min, outputRange.max, false);
}

float ofxTimeline::normalizedXtoScreenX(float x, ofRange inputRange){
	return ofMap(x, inputRange.min, inputRange.max, getDrawRect().x, getDrawRect().x+getDrawRect().width, false);
}
