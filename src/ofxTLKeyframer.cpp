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
#include "ofxTLUtils.h"

bool keyframesort(ofxTLKeyframe* a, ofxTLKeyframe* b){
	return a->position.x < b->position.x;
}

ofxTLKeyframer::ofxTLKeyframer(){
    
    hoverKeyframe = NULL;
	xmlFileName = "_keyframes.xml";	
	drawingSelectRect = false;
	keysAreDraggable = false;
}

ofxTLKeyframer::~ofxTLKeyframer(){
	clear();
}

void ofxTLKeyframer::setup(){
	enable();
	load();
}

//void ofxTLKeyframer::setValueRange(ofRange range, float newDefaultValue){
//	valueRange = range;
//    defaultValue = newDefaultValue;
//}
//
////main function to get values out of the timeline, operates on the given value range
//float ofxTLKeyframer::getValueAtPercent(float percent){
//	return ofMap(sampleAt(percent), 0.0, 1.0, valueRange.min, valueRange.max, false);
//}

//float ofxTLKeyframer::sampleAt(float percent){
//	percent = ofClamp(percent, 0, 1.0);
//	
//	//edge cases
//	if(keyframes.size() == 0){
//		return ofMap(defaultValue, valueRange.min, valueRange.max, 0, 1.0, true);
//	}
//	
//	if(percent < keyframes[0]->position.x){
//		return keyframes[0]->position.y;
//	}
//	
//	if(percent > keyframes[keyframes.size()-1]->position.x){
//		return keyframes[keyframes.size()-1]->position.y;
//	}
//	
//	for(int i = 1; i < keyframes.size(); i++){
//		if(keyframes[i]->position.x >= percent){
//			float percentBetween = ofxTween::map(percent, keyframes[i-1]->position.x, keyframes[i]->position.x, 0.0, 1.0, false, 
//												 *keyframes[i-1]->easeFunc->easing, keyframes[i-1]->easeType->type);
//			return keyframes[i-1]->position.y * (1.-percentBetween) + keyframes[i]->position.y*percentBetween;
//		}
//	}
//	
//	ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- Error condition, couldn't find keyframe for percent " + ofToString(percent, 4));
//	return defaultValue;
//}


void ofxTLKeyframer::draw(){
	//TODO: simple keyframe draw
}

//void ofxTLKeyframer::draw(){
//	
//	if(bounds.width == 0 || bounds.height == 0){
////		ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- Error condition, invalid bounds " + ofToString(bounds.width) + " " + ofToString(bounds.height) );
//		return;
//	}
//	
//	ofPushStyle();
//	ofPushMatrix();
//	ofEnableSmoothing();
//	
//	//**** DRAW BORDER. TODO: move to super class
//	ofNoFill();
//	if(hover){
//		ofSetColor(timeline->getColors().highlightColor);
//	}
//	else if(focused){
//		ofSetColor(timeline->getColors().highlightColor);
//	}
//	else{
//		ofSetColor(timeline->getColors().outlineColor);
//	}	
//	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
//	
//	
//	//**** DRAW KEYFRAME LINES
//	ofSetColor(timeline->getColors().highlightColor);
//	ofNoFill();
//	ofBeginShape();
//	if(keyframes.size() == 0 || keyframes.size() == 1){
//		ofVertex(bounds.x, bounds.y + bounds.height - sampleAt(.5)*bounds.height);
//		ofVertex(bounds.x+bounds.width, bounds.y + bounds.height - sampleAt(.5)*bounds.height);
//	}
//	else{
//		for(int p = 0; p <= bounds.width; p++){
//            //TODO: cache this into a poly line to avoid insane sampling.
//			ofVertex(bounds.x + p,  bounds.y + bounds.height - sampleAt(ofMap(p, 0, bounds.width, zoomBounds.min, zoomBounds.max, true)) * bounds.height);
//		}
//	}
//	ofEndShape(false);
//	
//	//**** DRAW KEYFRAME DOTS
//	ofSetColor(timeline->getColors().textColor);
//	for(int i = 0; i < keyframes.size(); i++){
//		if(!isKeyframeIsInBounds(keyframes[i])){
//			continue;
//		}
//		
//		ofSetColor(timeline->getColors().textColor);
//		ofVec2f screenpoint = coordForKeyframePoint(keyframes[i]->position);
//
//		if(isKeyframeSelected( keyframes[i] )){
//			float keysValue = ofMap(keyframes[i]->position.y, 0, 1.0, valueRange.min, valueRange.max, true);
//			string frameString = timeline->getIsFrameBased() ? 
//				ofToString(int(keyframes[i]->position.x*timeline->getDurationInFrames())) : 
//				timeline->formatTime(keyframes[i]->position.x*timeline->getDurationInSeconds());
//			ofDrawBitmapString(frameString+"|"+ofToString(keysValue, 4), screenpoint.x+5, screenpoint.y+10);
//			ofFill();
//		}
//		else{
//			ofNoFill();
//		}
//		
//		if(keyframes[i] == hoverKeyframe){
//			ofPushStyle();
//			ofFill();			
//			ofSetColor(timeline->getColors().highlightColor);
//			ofCircle(screenpoint.x, screenpoint.y, 8);
//			ofPopStyle();
//		}
//		
//		ofCircle(screenpoint.x, screenpoint.y, 4);
//	}
//	
//	//**** DRAW SELECT RECT
//	if(drawingSelectRect){
//		ofFill();
//		ofSetColor(timeline->getColors().keyColor, 30);
//		ofRect(selectRect);
//		
//		ofNoFill();
//		ofSetColor(timeline->getColors().keyColor, 255);
//		ofRect(selectRect);
//		
//	}
//	ofPopMatrix();
//	ofPopStyle();
//}



void ofxTLKeyframer::load(){
	ofxXmlSettings savedkeyframes;
    clear();
	
	if(!savedkeyframes.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- couldn't load xml file " + xmlFileName);
		return;
	}
	
	createKeyframesFromXML(savedkeyframes, keyframes);
    timeline->flagUserChangedValue();
	updateKeyframeSort();
}

void ofxTLKeyframer::createKeyframesFromXML(ofxXmlSettings xmlStore, vector<ofxTLKeyframe*>& keyContainer){


	int numKeyframeStores = xmlStore.getNumTags("keyframes");
	for(int store = 0; store < numKeyframeStores; store++){
		xmlStore.pushTag("keyframes",store);
		int numKeyTags = xmlStore.getNumTags("key");
		
		for(int i = 0; i < numKeyTags; i++){
			xmlStore.pushTag("key", i);
			ofxTLKeyframe* key = newKeyframe(ofVec2f(xmlStore.getValue("x", 0.0),
													 xmlStore.getValue("y", 0.0)));
			
			restoreKeyframe(key, xmlStore);			
            
			xmlStore.popTag(); //key
			
			keyContainer.push_back( key );
		}
		
		xmlStore.popTag();//keyframes
	}
	
	sort(keyContainer.begin(), keyContainer.end(), keyframesort);

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
	string xmlRep = getXMLStringForKeyframes(keyframes);
	ofxXmlSettings savedkeyframes;
	savedkeyframes.loadFromBuffer(xmlRep);
	savedkeyframes.saveFile(xmlFileName);
}

string ofxTLKeyframer::getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys){
	ofxXmlSettings savedkeyframes;
	savedkeyframes.addTag("keyframes");
	savedkeyframes.pushTag("keyframes");
	
	for(int i = 0; i < keys.size(); i++){
		savedkeyframes.addTag("key");
		savedkeyframes.pushTag("key", i);
		savedkeyframes.addValue("x", keys[i]->position.x);
		savedkeyframes.addValue("y", keys[i]->position.y);
		savedkeyframes.popTag(); //key
	}
	
	savedkeyframes.popTag();//keyframes
	string str;
	savedkeyframes.copyXmlToString(str);
	return str;
}

void ofxTLKeyframer::mousePressed(ofMouseEventArgs& args){
	ofVec2f screenpoint = ofVec2f(args.x, args.y);

	//TODO: move to modal click world
    /*
	if(drawingEasingWindow){
		//see if we clicked on an
		drawingEasingWindow = false;

		for(int i = 0; i < easingFunctions.size(); i++){
			if(easingFunctions[i]->bounds.inside(screenpoint-easingWindowPosition)){
				for(int k = 0; k < selectedKeyframes.size(); k++){
					selectedKeyframes[k]->easeFunc = easingFunctions[i];
				}
				if(autosave) save();
				return;
			}
		}
        
		for(int i = 0; i < easingTypes.size(); i++){
			if(easingTypes[i]->bounds.inside(screenpoint-easingWindowPosition)){
				for(int k = 0; k < selectedKeyframes.size(); k++){
					selectedKeyframes[k]->easeType = easingTypes[i];
				}
				if(autosave) save();
				return;
			}
		}
	}
    */
    
    keysAreDraggable = !ofGetModifierKeyShift();

    bool didJustDeselect = false;
	selectedKeyframe = keyframeAtScreenpoint(screenpoint, selectedKeyframeIndex);
    //if we clicked OFF of a keyframe OR...
    //if we clicked on a keyframe outside of the current selection and we aren't holding down shift, clear all
    if(hasFocus() && !ofGetModifierKeyShift()){
	    if( (selectedKeyframe == NULL && selectedKeyframes.size() != 0) || 
           	(selectedKeyframe != NULL && !isKeyframeSelected(selectedKeyframe)) ){
    	    timeline->unselectAll();
            didJustDeselect = true;
        }
        
        //if we didn't just deselect everything and clicked in an empty space add a new keyframe there
        if(selectedKeyframe == NULL && !didJustDeselect){
            //add a new one
            selectedKeyframe = newKeyframe( keyframePointForCoord(screenpoint) );
            keyframes.push_back(selectedKeyframe);
            selectedKeyframe->grabOffset = ofVec2f(0,0);
            timeline->flagUserChangedValue();
            updateKeyframeSort();
            
            for(int i = 0; i < keyframes.size(); i++){
                if(keyframes[i] == selectedKeyframe){
                    selectedKeyframeIndex = i;
                }
            }
        }
    }

    //add the keyframe to the selection, whether it was just generated or not
	if(selectedKeyframe != NULL){
        
    	if(!isKeyframeSelected(selectedKeyframe)){
			selectedKeyframes.push_back(selectedKeyframe);
        }
        //unselect it if it's selected and we clicked the key with shift pressed
        else if(ofGetModifierKeyShift()){
        	deselectKeyframe(selectedKeyframe);
        }
	}
	
    //if we have any keyframes selected update the grab offsets and check for showing the modal window
	if(selectedKeyframes.size() != 0){
        
        updateDragOffsets(screenpoint);				
		if(selectedKeyframe != NULL && args.button == 0){
			timeline->setDragAnchor(selectedKeyframe->grabOffset.x);
			//move the playhead
			if(timeline->getMovePlayheadOnDrag()){
				timeline->setPercentComplete(selectedKeyframe->position.x);
			}
		}
		else if(args.button == 2){
            selectedKeySecondaryClick(args);
//			easingWindowPosition = ofVec2f(MIN(screenpoint.x, bounds.width - easingBoxWidth),
//										   MIN(screenpoint.y, (timeline->getDrawRect().y + timeline->getDrawRect().height) - (easingBoxHeight*easingFunctions.size() + easingBoxHeight*easingTypes.size())));
//			drawingEasingWindow = true;
		}
	}
}

// MODAL CLICK TEST
// GET SELECTED ITEM
// CREATE NEW ITEM
// RIGHT CLICK ITEM

void ofxTLKeyframer::updateDragOffsets(ofVec2f screenpoint){
	for(int k = 0; k < selectedKeyframes.size(); k++){
		selectedKeyframes[k]->grabOffset = screenpoint - coordForKeyframePoint(selectedKeyframes[k]->position);
	}
}

void ofxTLKeyframer::mouseMoved(ofMouseEventArgs& args){
	ofxTLTrack::mouseMoved(args);
	int unused;
	hoverKeyframe = keyframeAtScreenpoint( ofVec2f(args.x, args.y), unused );
}

void ofxTLKeyframer::mouseDragged(ofMouseEventArgs& args, bool snapped){
	

    if(drawingSelectRect){
        selectRect = ofRectangle(selectRectStartPoint.x, selectRectStartPoint.y, 
                                 args.x-selectRectStartPoint.x, args.y-selectRectStartPoint.y);
        if(selectRect.width < 0){
            selectRect.x = selectRect.x+selectRect.width;
            selectRect.width = -selectRect.width;
        }
        if(selectRect.height < 0){
            selectRect.y = selectRect.y+selectRect.height;
            selectRect.height = -selectRect.height;
        }
        
        //somehow get it to stop going above
        selectRect.width = MIN(selectRect.width, bounds.x+bounds.width-selectRect.x);
        selectRect.height = MIN(selectRect.height, bounds.y+bounds.height-selectRect.y);
        
        selectRectSelection.clear();
        for(int i = 0; i < keyframes.size(); i++){
            if( selectRect.inside(coordForKeyframePoint(keyframes[i]->position)) ){
                selectRectSelection.push_back( keyframes[i] );
            }
        }
    }
    else {
        if(selectedKeyframes.size() != 0 && keysAreDraggable){
            ofVec2f screenpoint(args.x,args.y);
            for(int k = 0; k < selectedKeyframes.size(); k++){
                ofVec2f newScreenPosition;
                newScreenPosition.x = screenpoint.x - selectedKeyframes[k]->grabOffset.x;
                newScreenPosition.y = screenpoint.y - selectedKeyframes[k]->grabOffset.y;
                ofVec2f newposition = keyframePointForCoord(newScreenPosition);
                selectedKeyframes[k]->position = newposition;
            }
            if(selectedKeyframe != NULL && timeline->getMovePlayheadOnDrag()){
                timeline->setPercentComplete(selectedKeyframe->position.x);
            }
            timeline->flagUserChangedValue();
            updateKeyframeSort();
        }
    }
}

void ofxTLKeyframer::updateKeyframeSort(){
	sort(keyframes.begin(), keyframes.end(), keyframesort);
}

void ofxTLKeyframer::mouseReleased(ofMouseEventArgs& args){
	if(drawingSelectRect){

		for(int i = 0; i < selectRectSelection.size(); i++){
			if(!isKeyframeSelected(selectRectSelection[i])){
				selectedKeyframes.push_back(selectRectSelection[i]);
			}
		}
		sort(selectedKeyframes.begin(), selectedKeyframes.end(), keyframesort);
		drawingSelectRect = false;
	}
		   
	if(autosave){
		save();
	}
}

void ofxTLKeyframer::getSnappingPoints(vector<float>& points){
	for(int i = 0; i < keyframes.size(); i++){
		if (isKeyframeIsInBounds(keyframes[i]) && !isKeyframeSelected(keyframes[i])) {
			points.push_back( coordForKeyframePoint(keyframes[i]->position).x );
			//points.push_back( keyframes[i]->position.x );
		}
	}
}

string ofxTLKeyframer::copyRequest(){
	return getXMLStringForKeyframes(selectedKeyframes);
}

string ofxTLKeyframer::cutRequest(){
	string xmlrep = getXMLStringForKeyframes(selectedKeyframes);
	deleteSelectedKeyframes();
	return xmlrep;	
}

void ofxTLKeyframer::pasteSent(string pasteboard){
	vector<ofxTLKeyframe*> keyContainer;
	ofxXmlSettings pastedKeys;
	if(pastedKeys.loadFromBuffer(pasteboard)){
		timeline->unselectAll();
		createKeyframesFromXML(pastedKeys, keyContainer);
		if(keyContainer.size() != 0){
			selectedKeyframes.clear();
			
			//normalize and add at playhead
			for(int i = 0; i < keyContainer.size(); i++){
				if(i != 0){
					keyContainer[i]->position.x -= keyContainer[0]->position.x;
					keyContainer[i]->position.x += timeline->getPercentComplete();
				}
				if(keyContainer[i]->position.x <= 1.0){
					selectedKeyframes.push_back(keyContainer[i]);
					keyframes.push_back(keyContainer[i]);
				}
				else{
					delete keyContainer[i];
				}
			}
			keyContainer[0]->position.x = timeline->getPercentComplete();
			if(timeline->getMovePlayheadOnPaste()){
				timeline->setPercentComplete( keyContainer[keyContainer.size()-1]->position.x );
			}
			updateKeyframeSort();
			if(autosave){
				save();
			}
		}
	}
}

void ofxTLKeyframer::selectAll(){
	selectedKeyframes = keyframes;
}

void ofxTLKeyframer::unselectAll(){
	selectedKeyframes.clear();
}

void ofxTLKeyframer::keyPressed(ofKeyEventArgs& args){
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		deleteSelectedKeyframes();
	}
}

void ofxTLKeyframer::nudgeBy(ofVec2f nudgePercent){
	for(int i = 0; i < selectedKeyframes.size(); i++){
		selectedKeyframes[i]->position.x = ofClamp(selectedKeyframes[i]->position.x + nudgePercent.x, 0, 1.0);
		selectedKeyframes[i]->position.y = ofClamp(selectedKeyframes[i]->position.y + nudgePercent.y, 0, 1.0);
	}
	
    timeline->flagUserChangedValue();
	updateKeyframeSort();
	
	if(autosave){
		save();
	}	
}

void ofxTLKeyframer::deleteSelectedKeyframes(){
	for(int i = keyframes.size() - 1; i >= 0; i--){
		//if (keyframes[i] == selectedKeyframe) {
		if(isKeyframeSelected(keyframes[i])){
			delete keyframes[i];
			keyframes.erase(keyframes.begin()+i);
		}
	}
	
	selectedKeyframes.clear();
    
    timeline->flagUserChangedValue();
	updateKeyframeSort();
	if(autosave){
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
    selectedIndex = -1;
	return NULL;
}

void ofxTLKeyframer::deselectKeyframe(ofxTLKeyframe* k){
	for(int i = 0; i < selectedKeyframes.size(); i++){
        if(selectedKeyframes[i] == k){
            selectedKeyframes.erase(selectedKeyframes.begin() + i);
        }
    }
}

bool ofxTLKeyframer::isKeyframeSelected(ofxTLKeyframe* k){
	
	if(k == NULL) return false;
	
	for(int i = 0; i < selectedKeyframes.size(); i++){
		if(selectedKeyframes[i] == k){
			return true;
		}
	}
	return false;
}

bool ofxTLKeyframer::isKeyframeIsInBounds(ofxTLKeyframe* key){
	if(zoomBounds.min == 0.0 && zoomBounds.max == 1.0) return true;
	
	return key->position.x >= zoomBounds.min && key->position.x <= zoomBounds.max;
}

ofVec2f ofxTLKeyframer::coordForKeyframePoint(ofVec2f keyframePoint){
	return ofVec2f(ofMap(keyframePoint.x, zoomBounds.min, zoomBounds.max, bounds.x, bounds.x+bounds.width, false),
				   ofMap(keyframePoint.y, 1.0, 0.0, bounds.y, bounds.y+bounds.height, true));
}

bool ofxTLKeyframer::screenpointIsInBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

ofVec2f ofxTLKeyframer::keyframePointForCoord(ofVec2f coord){
	return ofVec2f(ofMap(coord.x, bounds.x, bounds.x+bounds.width,  zoomBounds.min, zoomBounds.max, false),
				   ofMap(coord.y, bounds.y, bounds.y+bounds.height, 1.0, 0.0, true));
}

ofxTLKeyframe* ofxTLKeyframer::newKeyframe(ofVec2f point){
	ofxTLKeyframe* k = new ofxTLKeyframe();
	k->position = point;
//	k->easeFunc = easingFunctions[0];
//	k->easeType = easingTypes[0];
	return k;
}
