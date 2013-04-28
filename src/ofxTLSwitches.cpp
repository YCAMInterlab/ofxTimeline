/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
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
 */

#include "ofxTLSwitches.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"

ofxTLSwitches::ofxTLSwitches(){
	placingSwitch = NULL;
}

ofxTLSwitches::~ofxTLSwitches(){
    
}

void ofxTLSwitches::draw(){
    
    ofPushStyle();
	ofFill();
	
	//draw a little wobble if its on
	//if(isOnAtMillis(timeline->getCurrentTimeMillis())){
	//play solo change
	if(isOn()){
		ofSetColor(timeline->getColors().disabledColor, 20+(1-powf(sin(ofGetElapsedTimef()*5)*.5+.5,2))*20);
		ofRect(bounds);
	}

    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        float startScreenX = MAX(millisToScreenX(switchKey->timeRange.min), 0);
        float endScreenX = MIN(millisToScreenX(switchKey->timeRange.max), bounds.getMaxX());
		if(startScreenX == endScreenX){
			continue;
		}
		switchKey->display = ofRectangle(startScreenX, bounds.y, endScreenX-startScreenX, bounds.height);

        //draw handles

        ofSetLineWidth(2);
        bool keyIsSelected = isKeyframeSelected(switchKey);
        if(keyIsSelected || switchKey->startSelected){
	        ofSetColor(timeline->getColors().textColor);
        }
        else{
	        ofSetColor(timeline->getColors().keyColor);    
        }

        ofLine(switchKey->display.x, bounds.y, 
               switchKey->display.x, bounds.y+bounds.height);

        if(keyIsSelected || switchKey->endSelected){
	        ofSetColor(timeline->getColors().textColor);                
        }
        else{
	        ofSetColor(timeline->getColors().keyColor);    
        }        
        ofLine(switchKey->display.x+switchKey->display.width, bounds.y, 
               switchKey->display.x+switchKey->display.width, bounds.y+bounds.height);

        //draw region
        if(keyIsSelected){
        	ofSetColor(timeline->getColors().textColor, 100);    
        }
        else{
        	ofSetColor(timeline->getColors().keyColor, 100);
        }
        //set overlay colors, this will override the colors above
        if(hoverKeyframe == switchKey){
            if(startHover){
                ofPushStyle();
                if(switchKey->startSelected){
                    ofSetColor(timeline->getColors().highlightColor);
                }
                else{
                    ofSetColor(timeline->getColors().keyColor);
                }
                ofRect(switchKey->display.x-2, bounds.y, 4, bounds.height);
                ofPopStyle();
            }
            else if(endHover){
				ofPushStyle();
                if(switchKey->endSelected){
                    ofSetColor(timeline->getColors().highlightColor);
                }
                else{
                    ofSetColor(timeline->getColors().keyColor);
                }
                ofRect(switchKey->display.x+switchKey->display.width-2, bounds.y, 4.0, bounds.height);
                ofPopStyle();
            }
            else {
                if(keyIsSelected){
	                ofSetColor(timeline->getColors().highlightColor);                    
                }else {
	                ofSetColor(timeline->getColors().keyColor);    
                }
            }
        }
        ofRect(switchKey->display);
    }
    ofPopStyle();
}

bool ofxTLSwitches::isOnAtMillis(long millis){
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if(switchKey->timeRange.min > millis){
            break;
        }
        if(switchKey->timeRange.contains(millis)){
            return true;
        }
    }
    return false;    
}

bool ofxTLSwitches::isOn(){
	return isOnAtMillis(currentTrackTime());
}

bool ofxTLSwitches::isOnAtPercent(float percent){
    unsigned long long millis = percent*timeline->getDurationInMilliseconds();
    return isOnAtMillis(millis);
}

bool ofxTLSwitches::mousePressed(ofMouseEventArgs& args, long millis){
    
	if(placingSwitch != NULL){
		if(isActive() && args.button == 0){
			placingSwitch->timeRange.max = millis;
			updateTimeRanges();
		}
		else {
			deleteKeyframe(placingSwitch);
		}
		placingSwitch = NULL;
		return false;
	}
	
	keysAreDraggable = !ofGetModifierSelection();
	
    //check to see if we are close to any edges, if so select them
    bool startSelected = false;
    bool endSelected = false;
    int selectedKeyframeIndex;
    if(isActive() && args.button == 0){
        for(int i = 0; i < keyframes.size(); i++){
            
            ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
            //unselect everything else if we just clicked this edge without shift held down
            startSelected = abs(switchKey->display.x - args.x) < 10.0;
            if (startSelected && !switchKey->startSelected && !ofGetModifierSelection()) {
                timeline->unselectAll();
            }
            //Deselect the key if we clicked it already selected with shift held down
            if(ofGetModifierSelection() && ((startSelected && switchKey->startSelected) || isKeyframeSelected(switchKey))){
                switchKey->startSelected = false;    
            }
            else {
                switchKey->startSelected |= startSelected;
            }
            float endEdge = switchKey->display.x+switchKey->display.width;
            endSelected = abs(endEdge - args.x) < 10.0;
            //don't let them both be selected in one click!
            if(!startSelected && endSelected && !switchKey->endSelected && !ofGetModifierSelection()){
                timeline->unselectAll();
            }
            //Deselect the key if we clicked it already selected with shift held down
            if(ofGetModifierSelection() && ((endSelected && switchKey->endSelected) || isKeyframeSelected(switchKey))){
                switchKey->endSelected = false;    
            }
            else{
                switchKey->endSelected |= endSelected && !startSelected;
            }
            
            if(startSelected || endSelected){
				selectedKeyframeIndex = i;
                break;
            }        
        }
    }
    
    //update dragging and snapping if we clicked an edge
    updateEdgeDragOffsets(millis);
    if(endSelected || startSelected){
        ofxTLSwitch* selectedSwitch = (ofxTLSwitch*)keyframes[selectedKeyframeIndex];
        timeline->setDragTimeOffset(selectedSwitch->edgeDragOffset);
    }
	
    if(!endSelected && !startSelected){
    	//normal selection from above

	    ofxTLKeyframes::mousePressed(args, millis);
        if(isActive()){
	        timeline->cancelSnapping(); //don't snap when dragging the whole switch
        }
    }
    
    //move through the keyframes, if both the start and the end have been selected
    //count it as completely selected and let the super class take care of it
    //otherwise if just one of the edges are selected make sure it's unselected
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if (switchKey->startSelected && switchKey->endSelected) {
            switchKey->startSelected = switchKey->endSelected = false;
            selectKeyframe(switchKey);
        }
        //make sure that if just one of the edges is clicked that the keyframe is *not* selected
		//also make sure it wasn't *just* selected in the last click by checking that it's not 'the' selected key
        else if( (switchKey->startSelected || switchKey->endSelected) && isKeyframeSelected(switchKey)){
			if(selectedKeyframe == switchKey){
				switchKey->startSelected = switchKey->endSelected = false;
			}
			else{
	            deselectKeyframe(switchKey);
			}
        }
    }
	return false;
}

void ofxTLSwitches::unselectAll(){
    ofxTLKeyframes::unselectAll();
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        switchKey->startSelected = switchKey->endSelected = false;
    }
}

void ofxTLSwitches::updateEdgeDragOffsets(long clickMillis){
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
    	if(switchKey->startSelected){
            switchKey->edgeDragOffset = clickMillis - switchKey->timeRange.min;
        }
        if( switchKey->endSelected){
            switchKey->edgeDragOffset = clickMillis - switchKey->timeRange.max;
        }
    }
}

void ofxTLSwitches::mouseDragged(ofMouseEventArgs& args, long millis){
        
    //do the normal dragging behavior for selected keyframes
    ofxTLKeyframes::mouseDragged(args, millis);
	
	if(keysAreDraggable){
		//look for any keys with just beginning and ends selected
		//becaues of the logical in the mousePressed, there will never
		//be a selected keyframe with an end selected
		for(int i = 0; i < keyframes.size(); i++){
			ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
			if(switchKey->startSelected){
				switchKey->timeRange.min = millis - switchKey->edgeDragOffset;
				switchKey->time = switchKey->timeRange.min;
			}
			else if(switchKey->endSelected){
				switchKey->timeRange.max = millis - switchKey->edgeDragOffset;
			}
		}
		
		updateTimeRanges();
	}
}

void ofxTLSwitches::mouseMoved(ofMouseEventArgs& args, long millis){
    endHover = startHover = false;
    if(hover && placingSwitch != NULL){
		placingSwitch->timeRange.max = millis;
		return;
	}
	
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if(abs(switchKey->display.x - args.x) < 10.0 && bounds.inside(args.x,args.y)){
            hoverKeyframe = switchKey;
            startHover = true;
            return; //return cancels call to parent
        }
        float endEdge = switchKey->display.x+switchKey->display.width;
        if(abs(endEdge - args.x) < 10.0 && bounds.inside(args.x,args.y)){
            hoverKeyframe = switchKey;
            endHover = true;
            return; //cancels call to parent
        }
    }
    ofxTLKeyframes::mouseMoved(args, millis);
}

void ofxTLSwitches::nudgeBy(ofVec2f nudgePercent){
	//super class nudge for selected keys
	ofxTLKeyframes::nudgeBy(nudgePercent);
	
	for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if(switchKey->startSelected){
            switchKey->timeRange.min += nudgePercent.x*timeline->getDurationInMilliseconds();
            switchKey->time = switchKey->timeRange.min;
        }
        else if(switchKey->endSelected){
            switchKey->timeRange.max += nudgePercent.x*timeline->getDurationInMilliseconds();
		}
	}
	
	updateTimeRanges();
}

//needed to sync the time ranges from pasted keys
void ofxTLSwitches::pasteSent(string pasteboard){
	ofxTLKeyframes::pasteSent(pasteboard);
	updateTimeRanges();
}

//This is called after dragging or nudging, and let's us make sure
void ofxTLSwitches::updateTimeRanges(){
	
    //the superclass will move the ->time value with the drag
    //so we look at the selected keyframes values and see if their changed
    //if so update both the min and the max time so the control moves as a block
	for(int i = 0; i < selectedKeyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)selectedKeyframes[i];
        long dif = switchKey->time - switchKey->timeRange.min;
        switchKey->timeRange.min = switchKey->time;
        switchKey->timeRange.max += dif;
    }
    
	//we also want ot make sure that any nudges or drags that happened to only edge values
	//reversed the min/max relationship value and swap them really quick
    for(int i = 0; i < keyframes.size(); i++){
        //check to see if the user reversed the value and swap them really quick
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
		if(switchKey->timeRange.min > switchKey->timeRange.max){
            float tempPos = switchKey->timeRange.max;
            switchKey->timeRange.max = switchKey->timeRange.min;
            switchKey->timeRange.min = switchKey->time = tempPos;
            bool tempSelect = switchKey->startSelected;
            switchKey->startSelected = switchKey->endSelected;
            switchKey->endSelected = tempSelect;
        }
    }
	
    //TODO: no overlaps!!
}

void ofxTLSwitches::mouseReleased(ofMouseEventArgs& args, long millis){
	ofxTLKeyframes::mouseReleased(args, millis);
}

void ofxTLSwitches::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
    	if(switchKey->timeRange.intersects(timeRange)){
            selectKeyframe(switchKey);
        }
    }
}

void ofxTLSwitches::getSnappingPoints(set<unsigned long long>& points){
	for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
		if (isKeyframeIsInBounds(switchKey) && !isKeyframeSelected(switchKey) &&
            !switchKey->startSelected && !switchKey->endSelected) {
			points.insert(switchKey->timeRange.min);
            points.insert(switchKey->timeRange.max);
		}
	}
}

int ofxTLSwitches::getSelectedItemCount(){
	int numEdgesSelected = 0;
	for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
		if(switchKey->startSelected ||switchKey->endSelected){
			numEdgesSelected++;
		}
	}
	return ofxTLKeyframes::getSelectedItemCount() + numEdgesSelected;
}

ofxTLKeyframe* ofxTLSwitches::newKeyframe(){
    ofxTLSwitch* switchKey = new ofxTLSwitch();
    //in the case of a click, start at the mouse positiion
    //if this is being restored from XML, the next call to restore will override this with what is in the XML
    switchKey->timeRange.min = switchKey->timeRange.max = screenXToMillis(ofGetMouseX());
    switchKey->startSelected = false;
    switchKey->endSelected   = true; //true so you can drag the range to start with
	
	//for just placing a switch we'll be able to decide the end position
	placingSwitch = switchKey;
	
    return switchKey;
}

void ofxTLSwitches::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //pull the saved time into min, and our custom max value
    ofxTLSwitch* switchKey = (ofxTLSwitch*)key;
    switchKey->timeRange.min = switchKey->time;
    //
    string timecode = xmlStore.getValue("max", "00:00:00:000");
    if(timecode.find(":") == string::npos){
        switchKey->timeRange.max = ofToFloat(timecode) * timeline->getDurationInMilliseconds(); //Legacy max of 0-1
    }
    else{
		switchKey->timeRange.max = timeline->getTimecode().millisForTimecode(timecode);
    }
    //this is so freshly restored keys won't have ends selected but click keys will
    switchKey->startSelected = switchKey->endSelected = false;
	
	//a bit of a hack, but if 
	placingSwitch = NULL;
}

void ofxTLSwitches::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //push the time range into X/Y
    ofxTLSwitch* switchKey = (ofxTLSwitch* )key;
    switchKey->time = switchKey->timeRange.min;
	xmlStore.addValue("max", timeline->getTimecode().timecodeForMillis(switchKey->timeRange.max));
}

ofxTLKeyframe* ofxTLSwitches::keyframeAtScreenpoint(ofVec2f p){
	for(int i = 0; i < keyframes.size(); i++){
		ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
    	if(switchKey->display.inside(p)){
            return switchKey;
        }
    }
    return NULL;
}

string ofxTLSwitches::getTrackType(){
    return "Switches";
}