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
	
	ofPopStyle();
}

void ofxTLSwitcher::mousePressed(ofMouseEventArgs& args){
	ofVec2f screenpoint(args.x,args.y);
	
	updateDragOffsets(args.x);
	
	bool clickInRect = bounds.inside(screenpoint);
	if(clickInRect){
		if(!focused){
			focused = true;
			return;
		}
	}
	
	if(!clickInRect){
		if(!ofGetModifierKeyShift()){
			deselectAllSwitches();
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
		deselectAllSwitches();
		bool startAlreadySelected = clickedSwitchA->startSelected;
		bool endAlreadySelected = clickedSwitchA->endSelected;
//		if((didSelectedStartTime && !clickedSwitchA->startSelected) || (!didSelectedStartTime && clickedSwitchA->endSelected)){
//		}
		clickedSwitchA->startSelected = didSelectedStartTime || (ofGetModifierKeyShift() && startAlreadySelected);
		clickedSwitchA->endSelected   = !didSelectedStartTime || (ofGetModifierKeyShift() && endAlreadySelected);
//		shouldDeselect = ((!startAlreadySelected && clickedSwitchA->startSelected) || (!endAlreadySelected != clickedSwitchA->endSelected));
	}
	else{
		
		float normalizedCoord = screenXtoNormalizedX(args.x, zoomBounds);
		bool shouldCreateNewSwitch = false;
		clickedSwitchA = switchForScreenX(args.x);
		if(clickedSwitchA != NULL){
			cout << "clicked a switch!" << endl;
			//if we clicked the upper part, select that switch
			if(hoveringOn(args.y)){
				//if we haven't already selected these, flag deselect
				if(!clickedSwitchA->startSelected || !clickedSwitchA->endSelected){
					deselectAllSwitches();
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
					deselectAllSwitches();
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
			deselectAllSwitches();
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
				
				if(before != NULL) cout << "	before is  " << before->time << endl;
				if(after != NULL) cout << "	after  is  " << after->time  << endl;
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
				cout << "new is " << clickedSwitchA->time << endl;
			}
			
			switches.push_back( clickedSwitchA );
			sort(switches.begin(), switches.end(), switchsort);
		}
//		else if(shouldDeselect){
//			deselectAllSwitches();
//		}
	}
	
	updateDragOffsets(args.x);

//	if(clickedSwitch != NULL){
//
//		cout << "setting drag bounds! " << endl;
//
//		clickedSwitch->dragOffsets = clickedSwitch->time - screenpoint.x;
//		clickedSwitch->startSelected = didSelectedStartTime;
//		clickedSwitch->endSelected = !didSelectedStartTime;
//		
//		if(!isSwitchSelected(clickedSwitch)){
//			cout << "adding switch! " << endl;
//			selectedSwitches.push_back( clickedSwitch );
//		}
//	}
		
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

void ofxTLSwitcher::mouseDragged(ofMouseEventArgs& args){
	for(int i = 0; i < switches.size(); i++){
		if(switches[i]->startSelected){
			switches[i]->time.min = screenXtoNormalizedX(args.x, zoomBounds) + switches[i]->dragOffsets.min;
		}
		if(switches[i]->endSelected){
			switches[i]->time.max = screenXtoNormalizedX(args.x, zoomBounds) + switches[i]->dragOffsets.max;
		}		
	}
}

void ofxTLSwitcher::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLSwitcher::keyPressed(ofKeyEventArgs& args){
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
	
}

void ofxTLSwitcher::load(){
	
}

void ofxTLSwitcher::clear(){
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
		switches[i]->dragOffsets = switches[i]->time - screenXtoNormalizedX(clickX, zoomBounds);
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

void ofxTLSwitcher::deselectAllSwitches(){
	if(ofGetModifierKeyShift()){
		return;
	}
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

//bool ofxTLSwitcher::isSwitchSelected(ofxTLSwitchOn* s){
//	for(int i = 0; i < selectedSwitches.size(); i++){
//		if(selectedSwitches[i] == s){
//			return true;
//		}
//	}
//	return false;
//}

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