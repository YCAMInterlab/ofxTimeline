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
	return a->time < b->time;
}

ofxTLKeyframer::ofxTLKeyframer() 
:	hoverKeyframe(NULL),
	keysAreDraggable(false),
	keysDidDrag(false),
	keysDidNudge(false)
{
	xmlFileName = "_keyframes.xml";	
}

ofxTLKeyframer::~ofxTLKeyframer(){
	clear();
}

void ofxTLKeyframer::draw(){
	//TODO: simple keyframe draw
}

void ofxTLKeyframer::load(){
	ofxXmlSettings savedkeyframes;
    clear();
	
	if(!savedkeyframes.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- couldn't load xml file " + xmlFileName);
		return;
	}
	
	createKeyframesFromXML(savedkeyframes, keyframes);
	timeline->flagTrackModified(this);
	updateKeyframeSort();
}

void ofxTLKeyframer::createKeyframesFromXML(ofxXmlSettings xmlStore, vector<ofxTLKeyframe*>& keyContainer){

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
                ofLogNotice() << "ofxTLKeyframer::createKeyframesFromXML -- Found legacy time " + legacyX << endl;
                float normalizedTime = ofToFloat(legacyX);
                key->time = normalizedTime*timeline->getDurationInMilliseconds();
            }
            else {
                string timecode = xmlStore.getValue("time", "00:00:00:000");
	            key->time = timeline->getTimecode().millisForTimecode(timecode);    
            }
            
            float legacyYValue = xmlStore.getValue("y", 0.0);
            if(legacyYValue != 0.0){
                ofLogNotice() << "ofxTLKeyframer::createKeyframesFromXML -- Found legacy value " << legacyYValue << endl;
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

void ofxTLKeyframer::clear(){
	for(int i = 0; i < keyframes.size(); i++){
		delete keyframes[i];
	}
	keyframes.clear();		
    selectedKeyframes.clear();
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

void ofxTLKeyframer::mousePressed(ofMouseEventArgs& args, long millis){
	ofVec2f screenpoint = ofVec2f(args.x, args.y);
    
    keysAreDraggable = !ofGetModifierKeyShift();
    keysDidDrag = false;

    bool didJustDeselect = false;
	selectedKeyframe = keyframeAtScreenpoint(screenpoint, selectedKeyframeIndex);
    //if we clicked OFF of a keyframe OR...
    //if we clicked on a keyframe outside of the current selection and we aren't holding down shift, clear all
    if(isActive() && !ofGetModifierKeyShift()){
	    if( (selectedKeyframe == NULL && selectedKeyframes.size() != 0) || 
           	(selectedKeyframe != NULL && !isKeyframeSelected(selectedKeyframe)) ){
    	    timeline->unselectAll();
            //debatable. settings this to true causes the first click off of the timeline to deselct rather than create a new keyframe
            //didJustDeselect = true; 
        }
        
        //if we didn't just deselect everything and clicked in an empty space add a new keyframe there
        if(selectedKeyframe == NULL && !didJustDeselect){
            timeline->unselectAll();

            //add a new one
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
		if(selectedKeyframe != NULL && args.button == 0){
            //timeline->setDragAnchor(selectedKeyframe->grabOffset.x);
            timeline->setDragTimeOffset(selectedKeyframe->grabTimeOffset);
			//move the playhead
			if(timeline->getMovePlayheadOnDrag()){
				timeline->setCurrentTimeMillis(selectedKeyframe->time);
			}
		}
		else if(args.button == 2){
            selectedKeySecondaryClick(args);
		}
	}
}

void ofxTLKeyframer::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
        cout << "time range is " << timeRange << " and keyframe is " << keyframes[i]->time << endl;
        if(timeRange.contains(keyframes[i]->time) && valueRange.contains(1.-keyframes[i]->value)){
            selectKeyframe(keyframes[i]);
        }
	}
}

void ofxTLKeyframer::updateDragOffsets(ofVec2f screenpoint, long grabMillis){
	for(int k = 0; k < selectedKeyframes.size(); k++){
		//selectedKeyframes[k]->grabOffset = screenpoint - coordForKeyframePoint(selectedKeyframes[k]->position);
        selectedKeyframes[k]->grabTimeOffset  = grabMillis - selectedKeyframes[k]->time;
        selectedKeyframes[k]->grabValueOffset = screenpoint.y - valueToScreenY(selectedKeyframes[k]->value);
	}
}

void ofxTLKeyframer::mouseMoved(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mouseMoved(args, millis);
	int unused;
	hoverKeyframe = keyframeAtScreenpoint( ofVec2f(args.x, args.y), unused );
}

void ofxTLKeyframer::mouseDragged(ofMouseEventArgs& args, long millis){
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

void ofxTLKeyframer::updateKeyframeSort(){
	sort(keyframes.begin(), keyframes.end(), keyframesort);
}

void ofxTLKeyframer::mouseReleased(ofMouseEventArgs& args, long millis){
	keysAreDraggable = false;
    if(keysDidDrag){
        timeline->flagTrackModified(this);
    }
}

void ofxTLKeyframer::getSnappingPoints(vector<long>& points){
	for(int i = 0; i < keyframes.size(); i++){
		if (isKeyframeIsInBounds(keyframes[i]) && !isKeyframeSelected(keyframes[i])) {
			points.push_back(keyframes[i]->time);
			//points.push_back( coordForKeyframePoint(keyframes[i]->position).x );
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

void ofxTLKeyframer::selectAll(){
	selectedKeyframes = keyframes;
}

void ofxTLKeyframer::unselectAll(){
	selectedKeyframes.clear();
}

int ofxTLKeyframer::getSelectedItemCount(){
    return selectedKeyframes.size();
}

string ofxTLKeyframer::getXMLRepresentation(){
    return getXMLStringForKeyframes(keyframes);
}

void ofxTLKeyframer::loadFromXMLRepresentation(string rep){
    clear();
    ofxXmlSettings buffer;
    buffer.loadFromBuffer(rep);
    createKeyframesFromXML(buffer, keyframes);
    updateKeyframeSort();
    timeline->flagUserChangedValue();    //because this is only called in Undo we don't flag track modified
}


void ofxTLKeyframer::keyPressed(ofKeyEventArgs& args){
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		deleteSelectedKeyframes();
	}
}

void ofxTLKeyframer::nudgeBy(ofVec2f nudgePercent){
	for(int i = 0; i < selectedKeyframes.size(); i++){
		selectedKeyframes[i]->time  = ofClamp(selectedKeyframes[i]->time + timeline->getDurationInMilliseconds()*nudgePercent.x, 0, timeline->getDurationInMilliseconds());
		selectedKeyframes[i]->value = ofClamp(selectedKeyframes[i]->value + nudgePercent.y, 0, 1.0);
	}
	
    timeline->flagTrackModified(this);
	updateKeyframeSort();	
}

void ofxTLKeyframer::deleteSelectedKeyframes(){
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

ofxTLKeyframe* ofxTLKeyframer::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
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

void ofxTLKeyframer::selectKeyframe(ofxTLKeyframe* k){
	if(!isKeyframeSelected(k)){
        selectedKeyframes.push_back(k);
    }
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
	long duration = timeline->getDurationInMilliseconds();
	return key->time >= zoomBounds.min*duration && key->time <= zoomBounds.max*duration;
}

ofVec2f ofxTLKeyframer::screenPositionForKeyframe(ofxTLKeyframe* keyframe){
    return ofVec2f(millisToScreenX(keyframe->time), 
                   valueToScreenY(keyframe->value));
}

bool ofxTLKeyframer::screenpointIsInBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

float ofxTLKeyframer::screenYToValue(float screenY){
    return ofMap(screenY, bounds.y, bounds.y+bounds.height, 1.0, 0.0, true);
}

float ofxTLKeyframer::valueToScreenY(float value){
	return ofMap(value, 1.0, 0.0, bounds.y, bounds.y+bounds.height, true);
}

ofxTLKeyframe* ofxTLKeyframer::newKeyframe(){
	ofxTLKeyframe* k = new ofxTLKeyframe();
	return k;
}

string ofxTLKeyframer::getTrackType(){
    return "Keyframes";
}