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

#include "ofxTLSwitcher.h"
#include "ofxTimeline.h"
#include "ofxTLUtils.h"

bool switchsort(ofxTLSwitchOn* a, ofxTLSwitchOn* b){
	return a->time.min < b->time.min;
}

ofxTLSwitcher::ofxTLSwitcher(){
	draggingSelectionRange = false;
	pointsAreDraggable = false;
}

ofxTLSwitcher::~ofxTLSwitcher(){
}

void ofxTLSwitcher::setup(){
	enable();
	load();	
}

void ofxTLSwitcher::update(ofEventArgs& args){
}

void ofxTLSwitcher::draw(){

	ofPushStyle();
	
	//**** DRAW BORDER
	ofNoFill();
	if(hover){
		ofSetColor(timeline->getColors().highlightColor);
	}
	else if(focused){
		ofSetColor(timeline->getColors().highlightColor);
	}
	else{
		ofSetColor(timeline->getColors().outlineColor);
	}	
	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	
	//draw the halfway dashed line
	for(int i = bounds.x; i < bounds.x+bounds.width; i += 10){
		ofSetColor(timeline->getColors().disabledColor);
		ofLine(i, bounds.y+bounds.height/2, i+5,bounds.y+bounds.height/2);
	}

	ofSetLineWidth(2);
	//iterate through the gaps drawing the off lines
	ofSetColor(timeline->getColors().disabledColor);
	float offY = bounds.y+bounds.height*.75;
	float onY  = bounds.y+bounds.height*.25;
	if(switches.size() == 0){
		//base case for no switches, everything is off
		ofLine(bounds.x, offY, bounds.x+bounds.width, offY);
	}
	else {
		
		for(int i = 0; i < switches.size()-1; i++){
			if(isSwitchInBounds(switches[i])){
				float startX = normalizedXtoScreenX( switches[i]->time.max, zoomBounds );
				float endX = normalizedXtoScreenX( switches[i+1]->time.min, zoomBounds );
				ofLine(startX, offY, endX, offY);
			}
		}
		
		//draw the start line
		if(isOn(zoomBounds.min)){
			ofxTLSwitchOn* startSwitch = switchForPoint(zoomBounds.min);
			
			//draw a line to the start
			ofSetColor(timeline->getColors().textColor);
			float startX = bounds.x;
			float endX = normalizedXtoScreenX( MIN(startSwitch->time.max, zoomBounds.max ), zoomBounds );
			ofLine(startX, onY, endX, onY);
		}
		else {
			
			ofSetColor(timeline->getColors().disabledColor);
			ofxTLSwitchOn* startSwitch = nearestSwitchAfterPoint(zoomBounds.min);
			float startX = bounds.x;
			float endX;
			if(startSwitch == NULL || startSwitch->time.min > zoomBounds.max){
				endX = bounds.x+bounds.width;
			}
			else{
				endX = normalizedXtoScreenX(startSwitch->time.min, zoomBounds);
			}
			ofLine(startX, offY, endX, offY);
		}
		
		//iterate through the switches, drawing the on lines
		ofxTLSwitchOn* lastSwitch = NULL;
		ofSetColor(timeline->getColors().textColor);
		for(int i = 0; i < switches.size(); i++){
			if(isSwitchInBounds(switches[i])){
				lastSwitch = switches[i];
				float startX = normalizedXtoScreenX( lastSwitch->time.min, zoomBounds );
				float endX = normalizedXtoScreenX( MIN(lastSwitch->time.max, zoomBounds.max), zoomBounds );
				ofLine(startX, onY, endX, onY);
			}
		}
		
		//draw the end bit if we need
		if(lastSwitch != NULL && lastSwitch->time.max < zoomBounds.max){
			ofSetColor(timeline->getColors().disabledColor);
			float startX = normalizedXtoScreenX( lastSwitch->time.max, zoomBounds );
			float endX = bounds.x+bounds.width;
			ofLine(startX, offY, endX, offY);
		}
		
		//draw the drag handles to connect the lines

		for(int i = 0; i < switches.size(); i++){
			if(zoomBounds.contains(switches[i]->time.min)){
				if(switches[i]->startSelected){
					ofSetColor(timeline->getColors().highlightColor);
				}
				else {
					ofSetColor(timeline->getColors().keyColor);
				}
				float x = normalizedXtoScreenX(switches[i]->time.min, zoomBounds); 
				ofLine(x, onY, x, offY);
			}
			if(zoomBounds.contains(switches[i]->time.max)){
				if(switches[i]->endSelected){
					ofSetColor(timeline->getColors().highlightColor);
				}
				else {
					ofSetColor(timeline->getColors().keyColor);
				}				
				float x = normalizedXtoScreenX(switches[i]->time.max, zoomBounds); 
				ofLine(x, onY, x, offY);				
			}   
		}
	}
	
	if(draggingSelectionRange){
		ofSetColor(timeline->getColors().keyColor);	
		ofLine(dragSelection.min, bounds.y, dragSelection.min, bounds.y+bounds.height);
		ofLine(dragSelection.max, bounds.y, dragSelection.max, bounds.y+bounds.height);
		ofSetColor(timeline->getColors().keyColor, 30);
		ofFill();
		ofRect(dragSelection.min, bounds.y, dragSelection.span(), bounds.height);
	}
	ofPopStyle();
}

void ofxTLSwitcher::mousePressed(ofMouseEventArgs& args){
	ofVec2f screenpoint(args.x,args.y);
	
	updateDragOffsets(args.x);
	pointsAreDraggable = !ofGetModifierKeyShift();

	bool clickInRect = bounds.inside(screenpoint);
	if(clickInRect){
		if(!focused){
			focused = true;
			return;
		}
	}
	
	if(!clickInRect){
		if(!ofGetModifierKeyShift()){
			//unselectAll();
			focused = false;
		}
		return;
	}
	
	bool shouldDeselect = false;
	bool didSelectedStartTime;
	//at this point we are inside the rect and focused, make changes
	ofxTLSwitchOn* clickedSwitchA = NULL;
	ofxTLSwitchOn* clickedSwitchB = NULL;
	clickedSwitchA = switchHandleForScreenX(args.x, didSelectedStartTime);
	if(clickedSwitchA != NULL){
		if(!ofGetModifierKeyShift()){
			timeline->unselectAll();
		}
		bool startAlreadySelected = clickedSwitchA->startSelected;
		bool endAlreadySelected = clickedSwitchA->endSelected;
		clickedSwitchA->startSelected = didSelectedStartTime || (ofGetModifierKeyShift() && startAlreadySelected);
		clickedSwitchA->endSelected   = !didSelectedStartTime || (ofGetModifierKeyShift() && endAlreadySelected);
		if(didSelectedStartTime){
			timeline->setDragAnchor( clickedSwitchA->dragOffsets.min );
		}
		else{
			timeline->setDragAnchor( clickedSwitchA->dragOffsets.max );
		}
	}
	else{
		if(ofGetModifierKeyShift()){
			draggingSelectionRange = true;
			selectionRangeAnchor = args.x;
			dragSelection.min = dragSelection.max = selectionRangeAnchor;
			return;
		}
		
		float normalizedCoord = screenXtoNormalizedX(args.x, zoomBounds);
		bool shouldCreateNewSwitch = false;
		clickedSwitchA = switchForScreenX(args.x);
		if(clickedSwitchA != NULL){

			//if we clicked the upper part, select that switch
			if(hoveringOn(args.y)){
				//if we haven't already selected these, flag deselect
				if((!clickedSwitchA->startSelected || !clickedSwitchA->endSelected) && !ofGetModifierKeyShift()){
					timeline->unselectAll();
				}
				clickedSwitchA->startSelected = true;
				clickedSwitchA->endSelected   = true;

			}
			else{
				//else make a new switch, splitting the existing one
				shouldCreateNewSwitch = true;
			}
		}
		else{
			//if we clicked below, select before and after
			if(!hoveringOn(args.y)){
				clickedSwitchA = nearestSwitchBeforePoint(normalizedCoord);
				clickedSwitchB = nearestSwitchAfterPoint(normalizedCoord);
				if( (clickedSwitchA != NULL && !clickedSwitchA->endSelected) || (clickedSwitchB != NULL && !clickedSwitchB->startSelected) ){
					//unselectAll();
					timeline->unselectAll();
				}
				
				if(clickedSwitchA != NULL){
					clickedSwitchA->endSelected = true;
				}
				if(clickedSwitchB != NULL){
					clickedSwitchB->startSelected = true;
				}
			}
			else{
				shouldCreateNewSwitch = true;
			}
		}
		
		//don't create new switches when shift is held down, and don't deselect anything
		if(ofGetModifierKeyShift()){
			shouldCreateNewSwitch = false;
			shouldDeselect = false;
		}
		
		//if we clicked where to create a new switch, but still have a selection, first deselect
		if(shouldCreateNewSwitch && areAnySwitchesSelected()){
			shouldCreateNewSwitch = false;
			//unselectAll();
			timeline->unselectAll();
		}
		
		if(shouldCreateNewSwitch){
			//make a new switch.
			clickedSwitchA = new ofxTLSwitchOn();
			clickedSwitchB = switchForPoint( normalizedCoord );
			//if we clicked in an off zone make a new switch there
			//or if we clicked inside a switch then we split that switch in half
			if(clickedSwitchB != NULL){
				clickedSwitchA->time.max = clickedSwitchB->time.max;
				clickedSwitchA->time.min = clickedSwitchB->time.min + clickedSwitchB->time.span() * .75;
				clickedSwitchB->time.max = clickedSwitchB->time.min + clickedSwitchB->time.span() * .25;
				clickedSwitchA->startSelected = true;
				clickedSwitchB->endSelected = true;
			}
			else{
				//fill up 50% to both edges, either the edge of the screen or up against the closets on switch
				ofxTLSwitchOn* before = nearestSwitchBeforePoint(normalizedCoord);
				ofxTLSwitchOn* after  = nearestSwitchAfterPoint(normalizedCoord);
				
//				if(before != NULL) cout << "	before is  " << before->time << endl;
//				if(after != NULL) cout << "	after  is  " << after->time  << endl;
				if(before == NULL || before->time.max < zoomBounds.min){
					clickedSwitchA->time.min = ofMap(.5, 0, 1.0, zoomBounds.min, normalizedCoord);
				}
				else {
					clickedSwitchA->time.min = ofMap(.5, 0, 1.0, before->time.max, normalizedCoord);
				}

				if(after == NULL || after->time.min > zoomBounds.max){
					clickedSwitchA->time.max = ofMap(.5, 0, 1.0, normalizedCoord, zoomBounds.max);
				}
				else {
					clickedSwitchA->time.max = ofMap(.5, 0, 1.0, normalizedCoord, after->time.min);
				}
				
				clickedSwitchA->endSelected = true;
				clickedSwitchA->startSelected = true;
			}
			
			switches.push_back( clickedSwitchA );
			sort(switches.begin(), switches.end(), switchsort);
			if(autosave) save();
		}
		updateDragOffsets(args.x);
	}
}

void ofxTLSwitcher::mouseMoved(ofMouseEventArgs& args){
	hoverSwitch = switchHandleForScreenX(args.x, hoveringStartTime);
	if(hoverSwitch == NULL){
		hoverSwitch = switchForScreenX(args.x);
		hoveringHandle = false;
	}
	else{
		hoveringHandle = true;
	}
}

//TODO: account for snapping
void ofxTLSwitcher::mouseDragged(ofMouseEventArgs& args, bool snapped){
	if(draggingSelectionRange){
		dragSelection.min = MIN(args.x, selectionRangeAnchor);
		dragSelection.max = MAX(args.x, selectionRangeAnchor);
	}
	
	else if(pointsAreDraggable){
	
		for(int i = 0; i < switches.size(); i++){
			if(switches[i]->startSelected){
				switches[i]->time.min = MIN( screenXtoNormalizedX(args.x - switches[i]->dragOffsets.min, zoomBounds), switches[i]->time.max);
			}
			if(switches[i]->endSelected){
				switches[i]->time.max = MAX( screenXtoNormalizedX(args.x - switches[i]->dragOffsets.max, zoomBounds), switches[i]->time.min);
			}
		}
		
		bool didSelectedStartTime;
		ofxTLSwitchOn* switchHandle = switchHandleForScreenX(args.x, didSelectedStartTime);
		if(timeline->getMovePlayheadOnPaste()){
			if(switchHandle != NULL){
				timeline->setPercentComplete(didSelectedStartTime ? switchHandle->time.min : switchHandle->time.max);
			}
			else{
				timeline->setPercentComplete(screenXtoNormalizedX(args.x, zoomBounds));
			}
		}
	}
}

void ofxTLSwitcher::mouseReleased(ofMouseEventArgs& args){
	if(draggingSelectionRange){
		for(int i = 0; i < switches.size(); i++){
			if(dragSelection.contains( normalizedXtoScreenX(switches[i]->time.min, zoomBounds))){
				switches[i]->startSelected = true;				
			}
			if(dragSelection.contains( normalizedXtoScreenX(switches[i]->time.max, zoomBounds))){
				switches[i]->endSelected = true;
			}
		}
	}
	
	for(int i = switches.size()-1; i >= 0; i--){
		if(switches[i]->time.min == switches[i]->time.max){
			delete switches[i];
			switches.erase(switches.begin()+i);
			
		}
	}	
	draggingSelectionRange = false;
	if(autosave) save();
}

void ofxTLSwitcher::keyPressed(ofKeyEventArgs& args){
	
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		
		//if an 'on' is selected, delete it
		for(int i = switches.size()-1; i >= 0; i--){
			if(switches[i]->startSelected && switches[i]->endSelected){
				delete switches[i];
				switches.erase(switches.begin()+i);
			}
		}
		//if an off valley is selected, merge the adjascent switches
		for(int i = switches.size()-1; i >= 1; i--){
			if(switches[i-1]->endSelected && switches[i]->startSelected){
				//merge
				switches[i-1]->time.max = switches[i]->time.max;
				delete switches[i];
				switches.erase(switches.begin()+i);
			}
		}
		if(autosave) save();
	}
}

void ofxTLSwitcher::nudgeBy(ofVec2f nudgePercent){
	for(int i = 0; i < switches.size(); i++){
		if(switches[i]->startSelected){
			switches[i]->time.min += nudgePercent.x; 
		}
		if(switches[i]->endSelected){
			switches[i]->time.max += nudgePercent.x; 
		}		
	}
	
	if(autosave) save();
}

bool ofxTLSwitcher::isOn(float percent){
	return switchForPoint(percent) != NULL;
}

ofxTLSwitchOn* ofxTLSwitcher::switchForScreenX(float screenPos){
	return switchForPoint(screenXtoNormalizedX(screenPos, zoomBounds));
}

ofxTLSwitchOn* ofxTLSwitcher::switchForPoint(float percent){
	for(int i = 0; i < switches.size(); i++){
		if(switches[i]->time.contains(percent)){
			return switches[i];
		}
	}
	return NULL;	
}

void ofxTLSwitcher::save(){
//	ofxXmlSettings settings;
//	settings.addTag("switches");
//	settings.pushTag("switches");
//	for(int i = 0; i < switches.size(); i++){
//		settings.addTag("switch");
//		settings.pushTag("switch",i);
//		settings.addValue("startTime", switches[i]->time.min);
//		settings.addValue("endTime", switches[i]->time.max);
//		settings.popTag();
//	}
//	settings.popTag();
//	settings.saveFile(xmlFileName);
	ofxXmlSettings settings;
	string xmlRep = getXMLStringForSwitches(false);
	settings.loadFromBuffer(xmlRep);
	settings.saveFile(xmlFileName);
}

void ofxTLSwitcher::load(){
	clear();
	ofxXmlSettings settings;
	if(settings.loadFile(xmlFileName)){
		switches = switchesFromXML(settings);
		
		//insertSwitchesFromXML(settings);
		
//		settings.pushTag("switches");
//		int numSwitches = settings.getNumTags("switch");
//		for(int i = 0; i < numSwitches; i++){
//			ofxTLSwitchOn* newSwitch = new ofxTLSwitchOn();
//			newSwitch->startSelected = newSwitch->endSelected = false;
//			settings.pushTag("switch", i);
//			newSwitch->time.min = settings.getValue("startTime",0.0);
//			newSwitch->time.max = settings.getValue("endTime",0.0);
//			switches.push_back(newSwitch);
//			settings.popTag();
//		}
//		settings.popTag();
//		sort(switches.begin(), switches.end(), switchsort);
	}
	else{
		ofLogError("ofxTLSwitcher -- Error loading from xml file " + xmlFileName);
	}
}

string ofxTLSwitcher::getXMLStringForSwitches(bool selectedOnly){
	ofxXmlSettings settings;
	settings.addTag("switches");
	settings.pushTag("switches");
	int switchesAdded = 0;
	for(int i = 0; i < switches.size(); i++){
		if(!selectedOnly || (switches[i]->startSelected && switches[i]->endSelected)){
			settings.addTag("switch");
			settings.pushTag("switch",switchesAdded);
			settings.addValue("startTime", switches[i]->time.min);
			settings.addValue("endTime", switches[i]->time.max);
			settings.popTag();
			switchesAdded++;
		}
	}
	settings.popTag();

	string ret;
	settings.copyXmlToString(ret);
//	cout << " xml request rep " << ret << endl;
	return ret;
}

vector<ofxTLSwitchOn*> ofxTLSwitcher::switchesFromXML(ofxXmlSettings xmlStore){
	vector<ofxTLSwitchOn*> newSwitches;

	int numSwitchStores = xmlStore.getNumTags("switches");
//	cout << " num switches " << numSwitchStores << endl;
	for(int s = 0; s < numSwitchStores; s++){
		xmlStore.pushTag("switches", s);
		int numSwitches = xmlStore.getNumTags("switch");

		for(int i = 0; i < numSwitches; i++){
			ofxTLSwitchOn* newSwitch = new ofxTLSwitchOn();
			newSwitch->startSelected = newSwitch->endSelected = false;
			xmlStore.pushTag("switch", i);
			newSwitch->time.min = xmlStore.getValue("startTime",0.0);
			newSwitch->time.max = xmlStore.getValue("endTime",0.0);
			newSwitches.push_back(newSwitch);
			xmlStore.popTag();
		}
		xmlStore.popTag();
	}
	
//	cout << "loaded " << newSwitches.size() << endl;
	
	sort(newSwitches.begin(), newSwitches.end(), switchsort);
	return newSwitches;
}

string ofxTLSwitcher::copyRequest(){
//	cout << "copy request" << endl;
	return getXMLStringForSwitches(true);
}

string ofxTLSwitcher::cutRequest(){
	string switchString = getXMLStringForSwitches(true);
	
	//if an 'on' is selected, delete it
	for(int i = switches.size()-1; i >= 0; i--){
		if(switches[i]->startSelected && switches[i]->endSelected){
			delete switches[i];
			switches.erase(switches.begin()+i);
		}
	}	
	
	if(autosave) save();
	
	return switchString;
}

void ofxTLSwitcher::pasteSent(string pasteboard){
//	cout << "pasting " << pasteboard << endl;
	ofxXmlSettings pasted;
	pasted.loadFromBuffer(pasteboard);
	vector<ofxTLSwitchOn*> newSwitches = switchesFromXML(pasted);
	if(newSwitches.size() == 0){
		return;
	}
	
	timeline->unselectAll();
	//move switches relative to the playhead
	float playheadPercent = timeline->getPercentComplete();
	float basePercent = newSwitches[0]->time.min;
	for(int i = 0; i < newSwitches.size(); i++){
//		cout << "pasted time is " << newSwitches[i]->time << endl;
		newSwitches[i]->time += playheadPercent - basePercent;
//		cout << "repositioned time " << newSwitches[i]->time << endl;
		newSwitches[i]->time.clamp(ofRange(0,1.0)); 
//		cout << "clamped time " << newSwitches[i]->time << endl;
	}
	
	//validate switch as not overlapping with any other switches
	float playheadJumpPoint = -1;
	for(int i = 0; i < newSwitches.size(); i++){
		for(int s = 0; s < switches.size(); s++){
			if(newSwitches[i]->time.intersects(switches[s]->time)){
				delete newSwitches[i];
				newSwitches[i] = NULL;
				break;
			}
		}
		if(newSwitches[i] != NULL){
			switches.push_back(newSwitches[i]);
			playheadJumpPoint = newSwitches[i]->time.max; 
		}
		
	}
	if(playheadJumpPoint != -1 && timeline->getMovePlayheadOnPaste()){
		timeline->setPercentComplete( playheadJumpPoint );
	}
	
	sort(switches.begin(), switches.end(), switchsort);
	if(autosave) save();
}

void ofxTLSwitcher::selectAll(){
	for(int i = 0; i < switches.size(); i++){
		switches[i]->startSelected = switches[i]->endSelected = true;
	}
//	for(int i = 0; i < switches.size(); i++){
//		if(zoomBounds.contains(switches[i]->time.min)){
//			switches[i]->startSelected = true;
//		}
//		if(zoomBounds.contains(switches[i]->time.max)){
//			switches[i]->endSelected = true;
//		}
//	}	
}

void ofxTLSwitcher::clear(){
	for(int i = 0; i < switches.size(); i++){
		delete switches[i];
	}
	switches.clear();
}

void ofxTLSwitcher::playbackStarted(ofxTLPlaybackEventArgs& args){
}
void ofxTLSwitcher::playbackLooped(ofxTLPlaybackEventArgs& args){
}
void ofxTLSwitcher::playbackEnded(ofxTLPlaybackEventArgs& args){
}

bool ofxTLSwitcher::isSwitchInBounds(ofxTLSwitchOn* s){
	return zoomBounds.intersects(s->time);
}
		
void ofxTLSwitcher::updateDragOffsets(float clickX){
	for(int i = 0; i < switches.size(); i++){
		switches[i]->dragOffsets = ofRange(clickX - normalizedXtoScreenX(switches[i]->time.min, zoomBounds),
										   clickX - normalizedXtoScreenX(switches[i]->time.max, zoomBounds));										   
	}
}

bool ofxTLSwitcher::areAnySwitchesSelected() {
	for(int i = 0; i < switches.size(); i++){
		if(switches[i]->startSelected || switches[i]->endSelected){
			return true;
		}
	}
	return false;
}

void ofxTLSwitcher::unselectAll(){
//	if(ofGetModifierKeyShift()){
//		return;
//	}
	
	for(int i = 0; i < switches.size(); i++){
		switches[i]->startSelected = false;
		switches[i]->endSelected = false;
	}
}

ofxTLSwitchOn* ofxTLSwitcher::switchHandleForScreenX(float screenPos, bool& startTimeSelected){
	for(int i = 0; i < switches.size(); i++){
		float xPos = normalizedXtoScreenX(switches[i]->time.min, zoomBounds);
		if(abs(xPos - screenPos) < 7){
			startTimeSelected = true;
			return switches[i];
		}
		xPos = normalizedXtoScreenX(switches[i]->time.max, zoomBounds);
		if(abs(xPos - screenPos) < 7){
			startTimeSelected = false;
			return switches[i];
		}
	}
	return NULL;
}


//Finds the closest switch behind a given point
//if the point is inside a switch, it will return NULL.
ofxTLSwitchOn* ofxTLSwitcher::nearestSwitchBeforePoint(float percent){
	float nearest = 1.0;
	ofxTLSwitchOn* nearestSwitch = NULL;
	for(int i = 0; i < switches.size(); i++){
		if (switches[i]->time.max > percent) {
			break;
		}
		if(percent - switches[i]->time.max < nearest){
			nearest = percent - switches[i]->time.max;
			nearestSwitch = switches[i];
		}
	}
	return nearestSwitch;
}

//Finds the closest switch after a given point
ofxTLSwitchOn* ofxTLSwitcher::nearestSwitchAfterPoint(float percent){
	float nearest = 1.0;
	ofxTLSwitchOn* nearestSwitch = NULL;
	for(int i = switches.size()-1; i >= 0; i--){
		if (switches[i]->time.max < percent) {
			break;
		}
		if(switches[i]->time.min - percent < nearest){
			nearest = switches[i]->time.min - percent;
			nearestSwitch = switches[i];
		}
	}
	return nearestSwitch;
}

bool ofxTLSwitcher::hoveringOn(float hoverY){
	return hoverY < bounds.y+bounds.height/2;
}