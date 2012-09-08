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

#include "ofxTLKeyframes.h"
#include "ofxTimeline.h"
//#include "ofxTLUtils.h"
#include "ofxHotKeys.h"

bool keyframesort(ofxTLKeyframe* a, ofxTLKeyframe* b){
	return a->time < b->time;
}

ofxTLKeyframes::ofxTLKeyframes()
:	hoverKeyframe(NULL),
	keysAreDraggable(false),
	keysDidDrag(false),
	keysDidNudge(false),
	lastKeyframeIndex(1),
	lastSampleTime(0),
	shouldRecomputePreviews(false),
	createNewOnMouseup(false)
{
	xmlFileName = "_keyframes.xml";	
}

ofxTLKeyframes::~ofxTLKeyframes(){
	clear();
}

void ofxTLKeyframes::recomputePreviews(){
	preview.clear();
	
//	cout << "ofxTLKeyframes::recomputePreviews " << endl;
	
	if(keyframes.size() == 0 || keyframes.size() == 1){
		//ofVertex(bounds.x, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height);
		preview.addVertex(ofPoint(bounds.x, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height));
//		ofVertex(bounds.x+bounds.width, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height);
		preview.addVertex(ofPoint(bounds.x+bounds.width, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height));
	}
	else{
		for(int p = bounds.x; p <= bounds.width; p++){
//			ofVertex(p,  bounds.y + bounds.height - sampleAtPercent(screenXtoNormalizedX(p)) * bounds.height);
			preview.addVertex(p,  bounds.y + bounds.height - sampleAtPercent(screenXtoNormalizedX(p)) * bounds.height);
		}
	}
//	int size = preview.getVertices().size();
	preview.simplify();
	//cout << "simplify pre " << size << " post: " << preview.getVertices().size() << " dif: " << (size - preview.getVertices().size()) << endl;

	ofVec2f lastPoint;
	keyPoints.clear();
	for(int i = 0; i < keyframes.size(); i++){
		if(!isKeyframeIsInBounds(keyframes[i])){
			continue;
		}
		ofVec2f screenpoint = screenPositionForKeyframe(keyframes[i]);
		if(lastPoint.squareDistance(screenpoint) > 5*5){
			keyPoints.push_back(screenpoint);
		}
		
		lastPoint = screenpoint;
	}
	
	shouldRecomputePreviews = false;
	
}

void ofxTLKeyframes::draw(){
	
	if(bounds.width == 0 || bounds.height == 0){
		return;
	}
	
	if(shouldRecomputePreviews || viewIsDirty){
		recomputePreviews();
	}
	
	ofPushStyle();
	
	//draw current value indicator as a big transparent rectangle
	ofSetColor(timeline->getColors().disabledColor, 30);
	float currentPercent = sampleAtTime(timeline->getCurrentTimeMillis());
	ofFill();
	ofRect(bounds.x, bounds.getMaxY(), bounds.width, -bounds.height*currentPercent);
	
	//***** DRAW KEYFRAME LINES
	ofSetColor(timeline->getColors().keyColor);
	ofNoFill();
	
	preview.draw();
	
	//**** DRAW KEYFRAME DOTS
	
	//**** HOVER FRAME
	if(hoverKeyframe != NULL){
		ofPushStyle();
		ofFill();
		ofSetColor(timeline->getColors().highlightColor);
		ofVec2f hoverKeyPoint = screenPositionForKeyframe( hoverKeyframe );
		ofCircle(hoverKeyPoint.x, hoverKeyPoint.y, 6);
		ofPopStyle();
	}

	//**** ALL CACHED VISIBLE KEYS
	ofSetColor(timeline->getColors().textColor);
	ofNoFill();
	for(int i = 0; i < keyPoints.size(); i++){
		ofRect(keyPoints[i].x-1, keyPoints[i].y-1, 3, 3);
	}
	
	//**** SELECTED KEYS
	ofSetColor(timeline->getColors().textColor);
	ofFill();
	for(int i = 0; i < selectedKeyframes.size(); i++){
		if(isKeyframeIsInBounds(selectedKeyframes[i])){
			ofVec2f screenpoint = screenPositionForKeyframe(selectedKeyframes[i]);
			float keysValue = ofMap(selectedKeyframes[i]->value, 0, 1.0, valueRange.min, valueRange.max, true);
			if(keysAreDraggable){
				string frameString = timeline->formatTime(selectedKeyframes[i]->time);
				ofDrawBitmapString(ofToString(keysValue, 4), screenpoint.x+5, screenpoint.y-5);
			}
			ofCircle(screenpoint.x, screenpoint.y, 4);
		}
	}

	ofPopStyle();
}

//TODO: potentially scale internal values at this point
void ofxTLKeyframes::setValueRange(ofRange range, float newDefaultValue){
	valueRange = range;
    defaultValue = newDefaultValue;
}

ofRange ofxTLKeyframes::getValueRange(){
	return valueRange;
}

//main function to get values out of the timeline, operates on the given value range
float ofxTLKeyframes::getValueAtPercent(float percent){
	//	return ofMap(sampleAt(percent), 0.0, 1.0, valueRange.min, valueRange.max, false);
    return getValueAtTimeInMillis(percent*timeline->getDurationInMilliseconds());
}

float ofxTLKeyframes::getValueAtTimeInMillis(long sampleTime){
	return ofMap(sampleAtTime(sampleTime), 0.0, 1.0, valueRange.min, valueRange.max, false);
}

float ofxTLKeyframes::sampleAtPercent(float percent){
	return sampleAtTime(percent * timeline->getDurationInMilliseconds());
}

float ofxTLKeyframes::sampleAtTime(long sampleTime){
	sampleTime = ofClamp(sampleTime, 0, timeline->getDurationInMilliseconds());
	
	//edge cases
	if(keyframes.size() == 0){
		return ofMap(defaultValue, valueRange.min, valueRange.max, 0, 1.0, true);
	}
	
	if(sampleTime < keyframes[0]->time){
		return keyframes[0]->value;
	}
	
	if(sampleTime > keyframes[keyframes.size()-1]->time){
		return keyframes[keyframes.size()-1]->value;
	}
	
	//optimization for linear playback
	int startKeyframeIndex = 1;
	if(sampleTime >= lastSampleTime){
		startKeyframeIndex = lastKeyframeIndex;
	}
	
	for(int i = startKeyframeIndex; i < keyframes.size(); i++){
		if(keyframes[i]->time >= sampleTime){
			lastKeyframeIndex = i;
			lastSampleTime = sampleTime;
			return interpolateValueForKeys(keyframes[i-1], keyframes[i], sampleTime);
		}
	}
	ofLog(OF_LOG_ERROR, "ofxTLKeyframes --- Error condition, couldn't find keyframe for percent " + ofToString(sampleTime));
	return defaultValue;
}

float ofxTLKeyframes::interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long sampleTime){
	return ofMap(sampleTime, start->time, end->time, start->value, end->value);
}

void ofxTLKeyframes::load(){
	ofxXmlSettings savedkeyframes;
    clear();
	
	if(!savedkeyframes.loadFile(xmlFileName)){
		ofLog(OF_LOG_NOTICE, "ofxTLKeyframes --- couldn't load xml file " + xmlFileName);
		return;
	}
	
	createKeyframesFromXML(savedkeyframes, keyframes);
	timeline->flagTrackModified(this);
	updateKeyframeSort();
}

void ofxTLKeyframes::createKeyframesFromXML(ofxXmlSettings xmlStore, vector<ofxTLKeyframe*>& keyContainer){

	int numKeyframeStores = xmlStore.getNumTags("keyframes");
	for(int store = 0; store < numKeyframeStores; store++){
		xmlStore.pushTag("keyframes",store);
		int numKeyTags = xmlStore.getNumTags("key");
		
		for(int i = 0; i < numKeyTags; i++){
			xmlStore.pushTag("key", i);
			//ofxTLKeyframe* key = newKeyframe(ofVec2f(xmlStore.getValue("x", 0.0),xmlStore.getValue("y", 0.0)));
            ofxTLKeyframe* key = newKeyframe();
            
            string legacyX = xmlStore.getValue("x", "");
            //if there is a decimal this is most likely an old save so let's 
            //convert it based on the current duration
            if(legacyX != ""){
                ofLogNotice() << "ofxTLKeyframes::createKeyframesFromXML -- Found legacy time " + legacyX << endl;
                float normalizedTime = ofToFloat(legacyX);
                key->time = normalizedTime*timeline->getDurationInMilliseconds();
            }
            else {
                string timecode = xmlStore.getValue("time", "00:00:00:000");
	            key->time = timeline->getTimecode().millisForTimecode(timecode);    
            }
            
            float legacyYValue = xmlStore.getValue("y", 0.0);
            if(legacyYValue != 0.0){
                ofLogNotice() << "ofxTLKeyframes::createKeyframesFromXML -- Found legacy value " << legacyYValue << endl;
                key->value = legacyYValue;
            }
            else{
	            key->value = xmlStore.getValue("value", 0.0f);
            }
			restoreKeyframe(key, xmlStore);			
			xmlStore.popTag(); //key
			keyContainer.push_back( key );
		}
		
		xmlStore.popTag(); //keyframes
	}
	sort(keyContainer.begin(), keyContainer.end(), keyframesort);
}

void ofxTLKeyframes::clear(){
	for(int i = 0; i < keyframes.size(); i++){
		delete keyframes[i];
	}
	keyframes.clear();
    selectedKeyframes.clear();
	updateKeyframeSort();
}

void ofxTLKeyframes::save(){
	string xmlRep = getXMLStringForKeyframes(keyframes);
	ofxXmlSettings savedkeyframes;
	savedkeyframes.loadFromBuffer(xmlRep);
	savedkeyframes.saveFile(xmlFileName);
}

string ofxTLKeyframes::getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys){
	ofxXmlSettings savedkeyframes;
	savedkeyframes.addTag("keyframes");
	savedkeyframes.pushTag("keyframes");
	
	for(int i = 0; i < keys.size(); i++){
		savedkeyframes.addTag("key");
		savedkeyframes.pushTag("key", i);
        
        //calling store before saving the default values gives the subclass a chance to modify them
        storeKeyframe(keys[i], savedkeyframes); 
		//savedkeyframes.addValue("x", keys[i]->position.x);
        //savedkeyframes.addValue("x", keys[i]->time);
        savedkeyframes.addValue("time", timeline->getTimecode().timecodeForMillis(keys[i]->time));
		savedkeyframes.addValue("value", keys[i]->value);
        
		savedkeyframes.popTag(); //key
	}
	
	savedkeyframes.popTag();//keyframes
	string str;
	savedkeyframes.copyXmlToString(str);
	return str;
}

bool ofxTLKeyframes::mousePressed(ofMouseEventArgs& args, long millis){
	ofVec2f screenpoint = ofVec2f(args.x, args.y);
    
	
    keysAreDraggable = !ofGetModifierShiftPressed();
    keysDidDrag = false;
	selectedKeyframe = keyframeAtScreenpoint(screenpoint);
    //if we clicked OFF of a keyframe OR...
    //if we clicked on a keyframe outside of the current selection and we aren't holding down shift, clear all
    if(!ofGetModifierSelection() && (isActive() || selectedKeyframe != NULL) ){
        bool didJustDeselect = false;
	    if( selectedKeyframe == NULL || !isKeyframeSelected(selectedKeyframe)){
            //settings this to true causes the first click off of the timeline to deselct rather than create a new keyframe
            didJustDeselect = timeline->getTotalSelectedItems() > 1;
    	    timeline->unselectAll();
        }

        //if we didn't just deselect everything and clicked in an empty space add a new keyframe there
        if(selectedKeyframe == NULL && !didJustDeselect){
			createNewOnMouseup = args.button == 0 && !ofGetModifierControlPressed();
        }
    }

   
	if(selectedKeyframe != NULL){
         //add the keyframe to the selection, whether it was just generated or not
    	if(!isKeyframeSelected(selectedKeyframe)){
			selectedKeyframes.push_back(selectedKeyframe);
        }
        //unselect it if it's selected and we clicked the key with shift pressed
        else if(ofGetModifierSelection()){
        	deselectKeyframe(selectedKeyframe);
        }
	}
	
    //if we have any keyframes selected update the grab offsets and check for showing the modal window
	if(selectedKeyframes.size() != 0){
        updateDragOffsets(screenpoint, millis);				
		if(selectedKeyframe != NULL && args.button == 0 && !ofGetModifierSelection()){
            timeline->setDragTimeOffset(selectedKeyframe->grabTimeOffset);
			//move the playhead
			if(timeline->getMovePlayheadOnDrag()){
				timeline->setCurrentTimeMillis(selectedKeyframe->time);
			}
		}
		else if(args.button == 2 || ofGetModifierControlPressed()){
            selectedKeySecondaryClick(args);
		}
	}
	return selectedKeyframe != NULL;
}

void ofxTLKeyframes::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
        if(timeRange.contains(keyframes[i]->time) && valueRange.contains(1.-keyframes[i]->value)){
            selectKeyframe(keyframes[i]);
        }
	}
}

void ofxTLKeyframes::updateDragOffsets(ofVec2f screenpoint, long grabMillis){
	for(int k = 0; k < selectedKeyframes.size(); k++){
		//selectedKeyframes[k]->grabOffset = screenpoint - coordForKeyframePoint(selectedKeyframes[k]->position);
        selectedKeyframes[k]->grabTimeOffset  = grabMillis - selectedKeyframes[k]->time;
        selectedKeyframes[k]->grabValueOffset = screenpoint.y - valueToScreenY(selectedKeyframes[k]->value);
	}
}

void ofxTLKeyframes::mouseMoved(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mouseMoved(args, millis);
	hoverKeyframe = keyframeAtScreenpoint( ofVec2f(args.x, args.y));
}

void ofxTLKeyframes::mouseDragged(ofMouseEventArgs& args, long millis){

    if(keysAreDraggable && selectedKeyframes.size() != 0){
        ofVec2f screenpoint(args.x,args.y);
        for(int k = 0; k < selectedKeyframes.size(); k++){
            ofVec2f newScreenPosition;
            selectedKeyframes[k]->time = millis - selectedKeyframes[k]->grabTimeOffset;
            selectedKeyframes[k]->value = screenYToValue(args.y - selectedKeyframes[k]->grabValueOffset);
            selectedKeyframes[k]->screenPosition = screenPositionForKeyframe(selectedKeyframes[k]);
        }
        if(selectedKeyframe != NULL && timeline->getMovePlayheadOnDrag()){
            timeline->setCurrentTimeMillis(selectedKeyframe->time);
        }
        timeline->flagUserChangedValue();
        keysDidDrag = true;
        updateKeyframeSort();
    }
	createNewOnMouseup = false;
}

void ofxTLKeyframes::updateKeyframeSort(){
	//reset these caches because they may no longer be valid
	shouldRecomputePreviews = true;
	lastKeyframeIndex = 1;
	lastSampleTime = 0;
	sort(keyframes.begin(), keyframes.end(), keyframesort);
}

void ofxTLKeyframes::mouseReleased(ofMouseEventArgs& args, long millis){
	keysAreDraggable = false;
    if(keysDidDrag){
		//reset these caches because they may no longer be valid
		lastKeyframeIndex = 1;
		lastSampleTime = 0;
        timeline->flagTrackModified(this);
    }
	
	if(createNewOnMouseup){
		//add a new one
		selectedKeyframe = newKeyframe();
		selectedKeyframe->time = millis;
		selectedKeyframe->value = screenYToValue(args.y);
		keyframes.push_back(selectedKeyframe);
		selectedKeyframes.push_back(selectedKeyframe);
		updateKeyframeSort();
//		cout << "creating new and  flagging" << endl;
		timeline->flagTrackModified(this);
	}
	createNewOnMouseup = false;
}

void ofxTLKeyframes::getSnappingPoints(set<long>& points){
	for(int i = 0; i < keyframes.size(); i++){
		if (isKeyframeIsInBounds(keyframes[i]) && !isKeyframeSelected(keyframes[i])) {
//			points.push_back(keyframes[i]->time);
			points.insert(keyframes[i]->time);
		}
	}
}

string ofxTLKeyframes::copyRequest(){
	return getXMLStringForKeyframes(selectedKeyframes);
}

string ofxTLKeyframes::cutRequest(){
	string xmlrep = getXMLStringForKeyframes(selectedKeyframes);
	deleteSelectedKeyframes();
	return xmlrep;	
}

void ofxTLKeyframes::pasteSent(string pasteboard){
	vector<ofxTLKeyframe*> keyContainer;
	ofxXmlSettings pastedKeys;
	if(pastedKeys.loadFromBuffer(pasteboard)){
		timeline->unselectAll();
		createKeyframesFromXML(pastedKeys, keyContainer);
		if(keyContainer.size() != 0){
			selectedKeyframes.clear();
			int numKeyframesPasted = 0;
			//normalize and add at playhead
			for(int i = 0; i < keyContainer.size(); i++){
				if(i != 0){
					keyContainer[i]->time -= keyContainer[0]->time;
					keyContainer[i]->time += timeline->getCurrentTimeMillis();
				}
				if(keyContainer[i]->time <= timeline->getDurationInMilliseconds()){
					selectedKeyframes.push_back(keyContainer[i]);
					keyframes.push_back(keyContainer[i]);
					numKeyframesPasted++;
				}
				else{
					delete keyContainer[i];
				}
			}
			
			if(numKeyframesPasted > 0){
				updateKeyframeSort();
				timeline->flagTrackModified(this);
			}
			
			keyContainer[0]->time = timeline->getCurrentTimeMillis();
			if(timeline->getMovePlayheadOnPaste()){
				timeline->setCurrentTimeMillis( keyContainer[keyContainer.size()-1]->time );
			}
		}
	}
}

void ofxTLKeyframes::addKeyframe(float value){
	addKeyframeAtMillis(value, timeline->getCurrentTimeMillis());
}

void ofxTLKeyframes::addKeyframeAtMillis(float value, unsigned long millis){
	ofxTLKeyframe* key = newKeyframe();
	key->time = millis;
	key->value = ofMap(value, valueRange.min, valueRange.max, 0, 1.0, true);
	keyframes.push_back(key);
	//smart sort, only sort if not added to end
	if(keyframes.size() > 2 && keyframes[keyframes.size()-2]->time > keyframes[keyframes.size()-1]->time){
		updateKeyframeSort();
	}
	lastKeyframeIndex = 1;
	timeline->flagTrackModified(this);
	shouldRecomputePreviews = true;
}

void ofxTLKeyframes::selectAll(){
	selectedKeyframes = keyframes;
}

void ofxTLKeyframes::unselectAll(){
	selectedKeyframes.clear();
}

int ofxTLKeyframes::getSelectedItemCount(){
    return selectedKeyframes.size();
}

string ofxTLKeyframes::getXMLRepresentation(){
    return getXMLStringForKeyframes(keyframes);
}

void ofxTLKeyframes::loadFromXMLRepresentation(string rep){
    clear();
    ofxXmlSettings buffer;
    buffer.loadFromBuffer(rep);
    createKeyframesFromXML(buffer, keyframes);
    updateKeyframeSort();
    timeline->flagUserChangedValue();    //because this is only called in Undo we don't flag track modified
}

void ofxTLKeyframes::keyPressed(ofKeyEventArgs& args){
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		deleteSelectedKeyframes();
	}
}

void ofxTLKeyframes::nudgeBy(ofVec2f nudgePercent){
	for(int i = 0; i < selectedKeyframes.size(); i++){
		selectedKeyframes[i]->time  = ofClamp(selectedKeyframes[i]->time + timeline->getDurationInMilliseconds()*nudgePercent.x, 0, timeline->getDurationInMilliseconds());
		selectedKeyframes[i]->value = ofClamp(selectedKeyframes[i]->value + nudgePercent.y, 0, 1.0);
	}
	
    timeline->flagTrackModified(this);
	updateKeyframeSort();	
}

void ofxTLKeyframes::deleteSelectedKeyframes(){
	for(int i = keyframes.size() - 1; i >= 0; i--){
		if(isKeyframeSelected(keyframes[i])){
			delete keyframes[i];
			keyframes.erase(keyframes.begin()+i);
		}
	}
	
	selectedKeyframes.clear();
	updateKeyframeSort();
    
    timeline->flagTrackModified(this);
}

void ofxTLKeyframes::deleteKeyframe(ofxTLKeyframe* keyframe){
	
	if(keyframe == NULL) return;
	
	for(int i = keyframes.size() - 1; i >= 0; i--){
		if(keyframe == keyframes[i]){
			deselectKeyframe(keyframe);
			delete keyframes[i];
			keyframes.erase(keyframes.begin()+i);
			return;
		}
	}
}

ofxTLKeyframe* ofxTLKeyframes::keyframeAtScreenpoint(ofVec2f p){
	if(!bounds.inside(p)){
		return NULL;	
	}
	float minDistanceSquared = 15*15;
	for(int i = 0; i < keyframes.size(); i++){
		ofVec2f keyonscreen = screenPositionForKeyframe(keyframes[i]);
		if(keyonscreen.squareDistance( p ) < minDistanceSquared){
			return keyframes[i];
		}
	}
	return NULL;
}

void ofxTLKeyframes::selectKeyframe(ofxTLKeyframe* k){
	if(!isKeyframeSelected(k)){
        selectedKeyframes.push_back(k);
    }
}

void ofxTLKeyframes::deselectKeyframe(ofxTLKeyframe* k){
	for(int i = 0; i < selectedKeyframes.size(); i++){
        if(selectedKeyframes[i] == k){
            selectedKeyframes.erase(selectedKeyframes.begin() + i);
			return;
        }
    }
}

bool ofxTLKeyframes::isKeyframeSelected(ofxTLKeyframe* k){
	
	if(k == NULL) return false;
	
	for(int i = 0; i < selectedKeyframes.size(); i++){
		if(selectedKeyframes[i] == k){
			return true;
		}
	}
	return false;
}

bool ofxTLKeyframes::isKeyframeIsInBounds(ofxTLKeyframe* key){
	if(zoomBounds.min == 0.0 && zoomBounds.max == 1.0) return true;
	long duration = timeline->getDurationInMilliseconds();
	return key->time >= zoomBounds.min*duration && key->time <= zoomBounds.max*duration;
}

ofVec2f ofxTLKeyframes::screenPositionForKeyframe(ofxTLKeyframe* keyframe){
    return ofVec2f(millisToScreenX(keyframe->time), 
                   valueToScreenY(keyframe->value));
}

bool ofxTLKeyframes::screenpointIsInBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

float ofxTLKeyframes::screenYToValue(float screenY){
    return ofMap(screenY, bounds.y, bounds.y+bounds.height, 1.0, 0.0, true);
}

float ofxTLKeyframes::valueToScreenY(float value){
	return ofMap(value, 1.0, 0.0, bounds.y, bounds.y+bounds.height, true);
}

ofxTLKeyframe* ofxTLKeyframes::newKeyframe(){
	ofxTLKeyframe* k = new ofxTLKeyframe();
	return k;
}

string ofxTLKeyframes::getTrackType(){
    return "Keyframes";
}