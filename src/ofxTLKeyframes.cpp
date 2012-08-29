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
#include "ofxTLUtils.h"

bool keyframesort(ofxTLKeyframe* a, ofxTLKeyframe* b){
	return a->time < b->time;
}

ofxTLKeyframes::ofxTLKeyframes()
:	hoverKeyframe(NULL),
	keysAreDraggable(false),
	keysDidDrag(false),
	keysDidNudge(false)
{
	xmlFileName = "_keyframes.xml";	
}

ofxTLKeyframes::~ofxTLKeyframes(){
	clear();
}

void ofxTLKeyframes::draw(){
	//TODO: simple keyframe draw
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

void ofxTLKeyframes::mousePressed(ofMouseEventArgs& args, long millis){
	ofVec2f screenpoint = ofVec2f(args.x, args.y);
    
    keysAreDraggable = !ofGetModifierKeyShift();
    keysDidDrag = false;

	selectedKeyframe = keyframeAtScreenpoint(screenpoint, selectedKeyframeIndex);
    //if we clicked OFF of a keyframe OR...
    //if we clicked on a keyframe outside of the current selection and we aren't holding down shift, clear all
    if(isActive() && !ofGetModifierKeyShift()){
        bool didJustDeselectMulti = false;
	    if( (selectedKeyframe == NULL && selectedKeyframes.size() != 0) || 
           	(selectedKeyframe != NULL && !isKeyframeSelected(selectedKeyframe)) ){
            //settings this to true causes the first click off of the timeline to deselct rather than create a new keyframe
            didJustDeselectMulti = timeline->getTotalSelectedItems() > 1;
    	    timeline->unselectAll();
        }
        
        //if we didn't just deselect everything and clicked in an empty space add a new keyframe there
        if(selectedKeyframe == NULL && !didJustDeselectMulti){
            timeline->unselectAll();

            //add a new one
            //TODO creating keyframes here breaks undo...
            selectedKeyframe = newKeyframe();
            selectedKeyframe->time = millis;
            selectedKeyframe->value = screenYToValue(screenpoint.y);
            keyframes.push_back(selectedKeyframe);
            //selectedKeyframe->grabOffset = ofVec2f(0,0);
            updateKeyframeSort();
            keysDidDrag = true; //triggers a save after mouseup
            for(int i = 0; i < keyframes.size(); i++){
                if(keyframes[i] == selectedKeyframe){
                    selectedKeyframeIndex = i;
                }
            }
        }
    }

   
	if(selectedKeyframe != NULL){
         //add the keyframe to the selection, whether it was just generated or not
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
        updateDragOffsets(screenpoint, millis);				
		if(selectedKeyframe != NULL && args.button == 0 && !ofGetModifierKeyControl()){
            timeline->setDragTimeOffset(selectedKeyframe->grabTimeOffset);
			//move the playhead
			if(timeline->getMovePlayheadOnDrag()){
				timeline->setCurrentTimeMillis(selectedKeyframe->time);
			}
		}
		else if(args.button == 2 || ofGetModifierKeyControl()){
            selectedKeySecondaryClick(args);
		}
	}
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
	int unused;
	hoverKeyframe = keyframeAtScreenpoint( ofVec2f(args.x, args.y), unused );
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
}

void ofxTLKeyframes::updateKeyframeSort(){
	sort(keyframes.begin(), keyframes.end(), keyframesort);
}

void ofxTLKeyframes::mouseReleased(ofMouseEventArgs& args, long millis){
	keysAreDraggable = false;
    if(keysDidDrag){
        timeline->flagTrackModified(this);
    }
}

void ofxTLKeyframes::getSnappingPoints(vector<long>& points){
	for(int i = 0; i < keyframes.size(); i++){
		if (isKeyframeIsInBounds(keyframes[i]) && !isKeyframeSelected(keyframes[i])) {
			points.push_back(keyframes[i]->time);
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
			
			//normalize and add at playhead
			for(int i = 0; i < keyContainer.size(); i++){
				if(i != 0){
					keyContainer[i]->time -= keyContainer[0]->time;
					keyContainer[i]->time += timeline->getCurrentTimeMillis();
				}
				if(keyContainer[i]->time <= timeline->getDurationInMilliseconds()){
					selectedKeyframes.push_back(keyContainer[i]);
					keyframes.push_back(keyContainer[i]);
				}
				else{
					delete keyContainer[i];
				}
			}
			keyContainer[0]->time = timeline->getCurrentTimeMillis();
			if(timeline->getMovePlayheadOnPaste()){
				timeline->setCurrentTimeMillis( keyContainer[keyContainer.size()-1]->time );
			}
			updateKeyframeSort();
		}
	}
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

ofxTLKeyframe* ofxTLKeyframes::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
	float minDistanceSquared = 15*15;
	for(int i = 0; i < keyframes.size(); i++){
		ofVec2f keyonscreen = screenPositionForKeyframe(keyframes[i]);
		if(keyonscreen.squareDistance( p ) < minDistanceSquared){
			selectedIndex = i;
			return keyframes[i];
		}
	}
    selectedIndex = -1;
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