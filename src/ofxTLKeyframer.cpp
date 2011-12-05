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

#include "ofxTLKeyframer.h"
#include "ofxTimeline.h"

bool keyframesort(ofxTLKeyframe* a, ofxTLKeyframe* b){
	return a->position.x < b->position.x;
}

ofxTLKeyframer::ofxTLKeyframer(){

	initializeEasings();
	reset();
	
	valueRange = ofRange(0.0, 1.0);
	hoverKeyframe = NULL;
	selectedKeyframe = NULL;
	drawingEasingWindow = false;
	
	xmlFileName = "_keyframes.xml";
	
	cout << "constructing keyframe editor" << endl;
	
}

ofxTLKeyframer::~ofxTLKeyframer(){
	clear();
}

void ofxTLKeyframer::setup(){
	cout << "setting up keyframe editor" << xmlFileName << endl;
	enable();
	load();
}

void ofxTLKeyframer::setValueRange(ofRange range){
	valueRange = range;
}

//main function to get values out of the timeline, operates on the given value range
float ofxTLKeyframer::getValueAtPercent(float percent){
	return ofMap(sampleAt(percent), 0.0, 1.0, valueRange.min, valueRange.max, false);
}

float ofxTLKeyframer::sampleAt(float percent){
	percent = ofClamp(percent, 0, 1.0);
	
	//edge cases
	if(keyframes.size() == 0){
		return .5;
	}
	
	if(percent < keyframes[0]->position.x){
		return keyframes[0]->position.y;
	}
	
	if(percent > keyframes[keyframes.size()-1]->position.x){
		return keyframes[keyframes.size()-1]->position.y;
	}
	
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->position.x >= percent){
			float percentBetween = ofxTween::map(percent, keyframes[i-1]->position.x, keyframes[i]->position.x, 0.0, 1.0, false, 
												 *keyframes[i-1]->easeFunc->easing, keyframes[i-1]->easeType->type);
			return keyframes[i-1]->position.y * (1.-percentBetween) + keyframes[i]->position.y*percentBetween;
		}
	}
	
	ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- Error condition, couldn't find keyframe for percent " + ofToString(percent, 4));
	return 0;
}


void ofxTLKeyframer::draw(){
	
	if(bounds.width == 0 || bounds.height == 0){
		ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- Error condition, invalid bounds " + ofToString(bounds.width) + " " + ofToString(bounds.height) );
		return;
	}
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableSmoothing();
	
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
	
	
	//**** DRAW KEYFRAME LINES
	ofSetColor(timeline->getColors().highlightColor);
	ofNoFill();
	ofBeginShape();
	if(keyframes.size() == 0 || keyframes.size() == 1){
		ofVertex(bounds.x, bounds.y + bounds.height - sampleAt(.5)*bounds.height);
		ofVertex(bounds.x+bounds.width, bounds.y + bounds.height - sampleAt(.5)*bounds.height);
	}
	else{
		for(int p = 0; p <= bounds.width; p++){
			ofVertex(bounds.x + p,  bounds.y + bounds.height - sampleAt(ofMap(p, 0, bounds.width, zoomBounds.min, zoomBounds.max, true)) * bounds.height);
		}
	}
	ofEndShape(false);
	
	//**** DRAW KEYFRAME DOTS
	ofSetColor(timeline->getColors().textColor);
	for(int i = 0; i < keyframes.size(); i++){
		if(!keyframeIsInBounds(keyframes[i])){
			continue;
		}
		
		ofSetColor(timeline->getColors().textColor);
		ofVec2f screenpoint = coordForKeyframePoint(keyframes[i]->position);
		if(keyframes[i] == selectedKeyframe){
			float keysValue = ofMap(selectedKeyframe->position.y, 0, 1.0, valueRange.min, valueRange.max, true);
			string frameString = timeline->getIsFrameBased() ? 
				ofToString(int(selectedKeyframe->position.x*timeline->getDurationInFrames())) : 
				ofToString(selectedKeyframe->position.y*timeline->getDurationInSeconds());
			ofDrawBitmapString(frameString+"|"+ofToString(keysValue, 2), screenpoint.x+5, screenpoint.y+10);
			ofFill();
		}
		else{
			ofNoFill();
		}
		
		if(keyframes[i] == hoverKeyframe){
			ofPushStyle();
			ofFill();			
			ofSetColor(timeline->getColors().highlightColor);
			ofCircle(screenpoint.x, screenpoint.y, 8);
			ofPopStyle();
		}
		
		ofCircle(screenpoint.x, screenpoint.y, 4);
	}
	
	//****** DRAW EASING CONTROLS
	if(drawingEasingWindow){
		for(int i = 0; i < easingFunctions.size(); i++){
			if(easingFunctions[i] == selectedKeyframe->easeFunc){
				ofSetColor(150, 100, 10);
			}
			else{
				ofSetColor(80, 80, 80);
			}
			ofFill();
			ofRect(easingWindowPosition.x + easingFunctions[i]->bounds.x, easingWindowPosition.y +easingFunctions[i]->bounds.y, 
				   easingFunctions[i]->bounds.width, easingFunctions[i]->bounds.height);
			ofSetColor(200, 200, 200);
			ofDrawBitmapString(easingFunctions[i]->name, 
							   easingWindowPosition.x + easingFunctions[i]->bounds.x+10, 
							   easingWindowPosition.y + easingFunctions[i]->bounds.y+15);			
			
			ofNoFill();
			ofSetColor(40, 40, 40);
			ofRect(easingWindowPosition.x + easingFunctions[i]->bounds.x, easingWindowPosition.y +easingFunctions[i]->bounds.y, 
				   easingFunctions[i]->bounds.width, easingFunctions[i]->bounds.height);
			
		}
		
		for(int i = 0; i < easingTypes.size(); i++){
			if(easingTypes[i] == selectedKeyframe->easeType){
				ofSetColor(150, 100, 10);
			}
			else{
				ofSetColor(80, 80, 80);
			}
			ofFill();
			ofRect(easingWindowPosition.x + easingTypes[i]->bounds.x, easingWindowPosition.y + easingTypes[i]->bounds.y, 
				   easingTypes[i]->bounds.width, easingTypes[i]->bounds.height);
			ofSetColor(200, 200, 200);
			ofDrawBitmapString(easingTypes[i]->name, 
							   easingWindowPosition.x + easingTypes[i]->bounds.x+10, 
							   easingWindowPosition.y + easingTypes[i]->bounds.y+15);			
			ofNoFill();
			ofSetColor(40, 40, 40);
			ofRect(easingWindowPosition.x + easingTypes[i]->bounds.x, 
				   easingWindowPosition.y + easingTypes[i]->bounds.y, 
				   easingTypes[i]->bounds.width, easingTypes[i]->bounds.height);
		}
	}
	
	ofPopMatrix();
	ofPopStyle();
}

void ofxTLKeyframer::load(){
	ofxXmlSettings savedkeyframes;
	
	cout << "Loading keyframe file " << xmlFileName << endl;
	
	if(!savedkeyframes.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- couldn't load xml file " + xmlFileName);
		reset();
		return;
	}
	
	clear();
	
	savedkeyframes.pushTag("keyframes");
	int numKeyTags = savedkeyframes.getNumTags("key");
	
	for(int i = 0; i < numKeyTags; i++){
		savedkeyframes.pushTag("key", i);
		ofxTLKeyframe* key = newKeyframe(ofVec2f(savedkeyframes.getValue("x", 0.0),
												 savedkeyframes.getValue("y", 0.0)));
								 
		key->easeFunc = easingFunctions[ofClamp(savedkeyframes.getValue("easefunc", 0), 0, easingFunctions.size()-1)];
		key->easeType = easingTypes[ofClamp(savedkeyframes.getValue("easetype", 0), 0, easingTypes.size()-1)];
										
		savedkeyframes.popTag(); //key

	}
	
	savedkeyframes.popTag();//keyframes
	
	updateKeyframeSort();
}

void ofxTLKeyframer::clear(){
	for(int i = 0; i < keyframes.size(); i++){
		delete keyframes[i];
	}
	keyframes.clear();	
}

void ofxTLKeyframer::reset(){
	clear();
}

void ofxTLKeyframer::save(){
	
	cout << "Saving keyframe file " << xmlFileName << endl;

	ofxXmlSettings savedkeyframes;
	savedkeyframes.addTag("keyframes");
	savedkeyframes.pushTag("keyframes");
	
	for(int i = 0; i < keyframes.size(); i++){
		savedkeyframes.addTag("key");
		savedkeyframes.pushTag("key", i);
		savedkeyframes.addValue("x", keyframes[i]->position.x);
		savedkeyframes.addValue("y", keyframes[i]->position.y);
		savedkeyframes.addValue("easefunc", keyframes[i]->easeFunc->id);
		savedkeyframes.addValue("easetype", keyframes[i]->easeType->id);
		savedkeyframes.popTag(); //key
	}
	
	savedkeyframes.popTag();//keyframes
	savedkeyframes.saveFile(xmlFileName);
}

void ofxTLKeyframer::mousePressed(ofMouseEventArgs& args){
	if(!enabled) return;
	
	ofVec2f screenpoint = ofVec2f(args.x, args.y);
	if(drawingEasingWindow){
		//see if we clicked on an
		drawingEasingWindow = false;

		for(int i = 0; i < easingFunctions.size(); i++){
			if(easingFunctions[i]->bounds.inside(screenpoint-easingWindowPosition)){
				selectedKeyframe->easeFunc = easingFunctions[i];
				if(autosave) save();
				return;
			}
		}
		for(int i = 0; i < easingTypes.size(); i++){
			if(easingTypes[i]->bounds.inside(screenpoint-easingWindowPosition)){
				selectedKeyframe->easeType = easingTypes[i];
				if(autosave) save();
				return;
			}
		}
	}
	
	bool clickIsInRect = screenpointIsInBounds(screenpoint);
	if(!focused){
		focused = clickIsInRect;
		if(!focused){
			selectedKeyframe = NULL;
			drawingEasingWindow = false;
		}
		return;
	}
	
	if(!clickIsInRect){
		focused = false;
		selectedKeyframe = NULL;
		drawingEasingWindow = false;
		return;
	}
	
	selectedKeyframe = keyframeAtScreenpoint(screenpoint, selectedKeyframeIndex);
	//cout << "selected index is " << selectedKeyframeIndex << endl;
	if(selectedKeyframe == NULL){
		//add a new one
		selectedKeyframe = newKeyframe( keyframePointForCoord(screenpoint) );
		
		grabOffset = ofVec2f(0,0);
		updateKeyframeSort();
		//find bounds
		for(int i = 0; i < keyframes.size(); i++){
			if(keyframes[i] == selectedKeyframe){
				selectedKeyframeIndex = i;
			}
		}
	}
	
	//grabbed a keyframe
	if(selectedKeyframe != NULL){
		timeline->setPercentComplete(selectedKeyframe->position.x);

		if(args.button == 0){
			grabOffset = screenpoint - coordForKeyframePoint(selectedKeyframe->position);
			if(keyframes.size() == 1){
				minBound = 0.0;
				maxBound = 1.0;
			}
			else if(selectedKeyframeIndex == 0){
				minBound = 0.0;
				maxBound = keyframes[selectedKeyframeIndex+1]->position.x;
			}
			else if(selectedKeyframeIndex == keyframes.size()-1){
				minBound = keyframes[selectedKeyframeIndex-1]->position.x;
				maxBound = 1.0;
			}
			else{
				minBound = keyframes[selectedKeyframeIndex-1]->position.x;
				maxBound = keyframes[selectedKeyframeIndex+1]->position.x;
				//cout << "keyframe point is " << selectedKeyframe->position.x << " min bound is " << minBound << " max bound is " << maxBound << endl;
			}
		}
		else if(args.button == 2){
			easingWindowPosition = screenpoint;
			drawingEasingWindow = true;
		}
	}
}

void ofxTLKeyframer::mouseMoved(ofMouseEventArgs& args){
	ofxTLElement::mouseMoved(args);
	int unused;
	hoverKeyframe = keyframeAtScreenpoint( ofVec2f(args.x, args.y), unused );
}

void ofxTLKeyframer::mouseDragged(ofMouseEventArgs& args){
	if(!enabled) return;
	
	if(focused && selectedKeyframe != NULL){
		
		ofVec2f newposition = keyframePointForCoord(ofVec2f(args.x, args.y) - grabOffset);
		newposition.x = ofClamp(newposition.x, minBound, maxBound);
		selectedKeyframe->position = newposition;
		timeline->setPercentComplete(newposition.x);
	}
}

void ofxTLKeyframer::updateKeyframeSort(){
	sort(keyframes.begin(), keyframes.end(), keyframesort);
}

void ofxTLKeyframer::mouseReleased(ofMouseEventArgs& args){
	if(pointInScreenBounds(ofVec2f(args.x, args.y))){
		if(autosave){
			save();
		}
	}
}

void ofxTLKeyframer::keyPressed(ofKeyEventArgs& args){
	if(!enabled || !focused) return;
	
	bool modified = false;
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		if(focused && selectedKeyframe != NULL){
			for(int i = 0; i < keyframes.size(); i++){
				if (keyframes[i] == selectedKeyframe) {
					delete keyframes[i];
					keyframes.erase(keyframes.begin()+i);
					selectedKeyframe = NULL;
					cout << "deleted keyframe at index " << i << endl;
					modified = true;
					break;
				}
			}
		}
	}
	if(focused && selectedKeyframe != NULL){
		
		if(args.key == OF_KEY_UP){
			selectedKeyframe->position.y = MIN( int(100*selectedKeyframe->position.y+1)/100.0, 1.0);
			modified = true;
		}
		
		if(args.key == OF_KEY_DOWN){
			selectedKeyframe->position.y = MAX( int(100*selectedKeyframe->position.y-1)/100.0, 0.0);			
			modified = true;
		}
		
		if(args.key == OF_KEY_RIGHT){
			selectedKeyframe->position.x = MIN(selectedKeyframe->position.x+.0001, maxBound);			
			modified = true;
		}
		if(args.key == OF_KEY_LEFT){
			selectedKeyframe->position.x = MAX(selectedKeyframe->position.x-.0001, minBound);			
			modified = true;
		}
	}
	
	if(autosave && modified){
		save();
	}
}

ofxTLKeyframe* ofxTLKeyframer::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
	float mindistance = 15;
	for(int i = 0; i < keyframes.size(); i++){
		ofVec2f keyonscreen = coordForKeyframePoint(keyframes[i]->position);
		if(keyonscreen.distance( p ) < mindistance){
			selectedIndex = i;
			return keyframes[i];
		}
	}
	return NULL;

}

bool ofxTLKeyframer::keyframeIsInBounds(ofxTLKeyframe* key){
	if(zoomBounds.min == 0.0 && zoomBounds.max == 1.0) return true;
	
	return key->position.x >= zoomBounds.min && key->position.x <= zoomBounds.max;
}

ofVec2f ofxTLKeyframer::coordForKeyframePoint(ofVec2f keyframePoint){
	return ofVec2f(ofMap(keyframePoint.x, zoomBounds.min, zoomBounds.max, bounds.x, bounds.x+bounds.width, true),
				   ofMap(keyframePoint.y, 1.0, 0.0, bounds.y, bounds.y+bounds.height, true));
}

bool ofxTLKeyframer::screenpointIsInBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

ofVec2f ofxTLKeyframer::keyframePointForCoord(ofVec2f coord){
	return ofVec2f(ofMap(coord.x, bounds.x, bounds.x+bounds.width,  zoomBounds.min, zoomBounds.max, true),
				   ofMap(coord.y, bounds.y, bounds.y+bounds.height, 1.0, 0.0, true));
}

ofxTLKeyframe* ofxTLKeyframer::newKeyframe(ofVec2f point){
	ofxTLKeyframe* k = new ofxTLKeyframe();
	k->position = point;
	k->easeFunc = easingFunctions[0];
	k->easeType = easingTypes[0];
	keyframes.push_back( k );
	return k;
}

void ofxTLKeyframer::initializeEasings(){

	//FUNCTIONS ----
	EasingFunction* ef;
	ef = new EasingFunction();
	ef->easing = new ofxEasingLinear();
	ef->name = "linear";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingSine();
	ef->name = "sine";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingCirc();
	ef->name = "circular";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingQuad();
	ef->name = "quadratic";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingCubic();
	ef->name = "cubic";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingQuart();
	ef->name = "quartic";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingQuint();
	ef->name = "quintic";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingExpo();
	ef->name = "exponential";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingBack();
	ef->name = "back";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingBounce();
	ef->name = "bounce";
	easingFunctions.push_back(ef);

	ef = new EasingFunction();
	ef->easing = new ofxEasingElastic();
	ef->name = "elastic";
	easingFunctions.push_back(ef);

	///TYPES -------
	EasingType* et;
	et = new EasingType();
	et->type = ofxTween::easeIn;
	et->name = "ease in";
	easingTypes.push_back(et);

	et = new EasingType();
	et->type = ofxTween::easeOut;
	et->name = "ease out";
	easingTypes.push_back(et);

	et = new EasingType();
	et->type = ofxTween::easeInOut;
	et->name = "ease in-out";
	easingTypes.push_back(et);
	
	//TODO: make configurable
	easingBoxWidth  = 120;
	easingBoxHeight = 20;
	easingWindowSeperatorHeight = 4;
	
	for(int i = 0; i < easingFunctions.size(); i++){
		easingFunctions[i]->bounds = ofRectangle(0, i*easingBoxHeight, easingBoxWidth, easingBoxHeight);
		easingFunctions[i]->id = i;
	}
	
	for(int i = 0; i < easingTypes.size(); i++){
		easingTypes[i]->bounds = ofRectangle(0, (i+easingFunctions.size())*easingBoxHeight + easingWindowSeperatorHeight, easingBoxWidth, easingBoxHeight);
		easingTypes[i]->id = i;
	}
}

