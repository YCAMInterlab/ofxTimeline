
#include "ofxTLSwitches.h"
#include "ofxTimeline.h"
//#include "ofxTLUtils.h"
#include "ofxHotKeys.h"

ofxTLSwitches::ofxTLSwitches(){
    edgeSelected = false;
}

ofxTLSwitches::~ofxTLSwitches(){
    
}

void ofxTLSwitches::draw(){
    
    ofPushStyle();
	ofFill();
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        float startScreenX = millisToScreenX(switchKey->timeRange.min);
        float endScreenX = millisToScreenX(switchKey->timeRange.max);
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

bool ofxTLSwitches::isOnAtPercent(float percent){
    unsigned long millis = percent*timeline->getDurationInMilliseconds();
    return isOnAtMillis(millis);
}

void ofxTLSwitches::mousePressed(ofMouseEventArgs& args, long millis){    
    
    //check to see if we are close to any edges, if so select them
    bool startSelected = false;
    bool endSelected = false;
    int selectedKeyframeIndex;
    if(isActive()){
        for(int i = 0; i < keyframes.size(); i++){
            
            ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
            //unselect everything else if we just clicked this edge without shift held down
            startSelected = abs(switchKey->display.x - args.x) < 10.0;
            if (startSelected && !switchKey->startSelected && !ofGetModifierShiftPressed()) {
                timeline->unselectAll();
            }
            //Deselect the key if we clicked it already selected with shift held down
            if(ofGetModifierShiftPressed() && ((startSelected && switchKey->startSelected) || isKeyframeSelected(switchKey))){
                switchKey->startSelected = false;    
            }
            else {
                switchKey->startSelected |= startSelected;
            }
            float endEdge = switchKey->display.x+switchKey->display.width;
            endSelected = abs(endEdge - args.x) < 10.0;
            //don't let them both be selected in one click!
            if(!startSelected && endSelected && !switchKey->endSelected && !ofGetModifierShiftPressed()){
                timeline->unselectAll();
            }
            //Deselect the key if we clicked it already selected with shift held down
            if(ofGetModifierShiftPressed() && ((endSelected && switchKey->endSelected) || isKeyframeSelected(switchKey))){
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
        else if( (switchKey->startSelected || switchKey->endSelected) && isKeyframeSelected(switchKey)){
            deselectKeyframe(switchKey);
        }
    }
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
    
    //update to make sure that any keyframes with edges selected are properly added or removed 
    
    //do the normal dragging behavior for selected keyframes
    ofxTLKeyframes::mouseDragged(args, millis);

    //the superclass will move the ->time value with the drag
    //so we look at the selected keyframes values and see if their changed
    //if so update both the min and the max time so the control moves as a block
	for(int i = 0; i < selectedKeyframes.size(); i++){
        ofxTLSwitch* switchKey =  (ofxTLSwitch*)selectedKeyframes[i];
        long dif = switchKey->time - switchKey->timeRange.min;
        switchKey->timeRange.min = switchKey->time;
        switchKey->timeRange.max += dif;
    }
    
    //now look for any keys with just beginning and ends selected
    //becaues of the logical in the mousePressed, there will never
    //be a selected keyframe with
	for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if(switchKey->startSelected){
            switchKey->timeRange.min = millis - switchKey->edgeDragOffset;
//            switchKey->timeRange.min = screenXToMillis(args.x - switchKey->edgeDragOffset);
            switchKey->time = switchKey->timeRange.min;
        }
        else if(switchKey->endSelected){
            switchKey->timeRange.max = millis - switchKey->edgeDragOffset;
//            switchKey->timeRange.max = screenXToMillis(args.x - switchKey->edgeDragOffset);
        }
    }

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

void ofxTLSwitches::mouseMoved(ofMouseEventArgs& args, long millis){
    
    endHover = startHover = false;
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
//        if(switchKey->display.x > args.x){
//            break;
//        }
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

void ofxTLSwitches::getSnappingPoints(set<long>& points){
	for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
		if (isKeyframeIsInBounds(switchKey) && !isKeyframeSelected(switchKey) &&
            !switchKey->startSelected && !switchKey->endSelected) {
			points.insert(switchKey->timeRange.min);
            points.insert(switchKey->timeRange.max);
		}
	}
}

ofxTLKeyframe* ofxTLSwitches::newKeyframe(){
    ofxTLSwitch* switchKey = new ofxTLSwitch();
    //in the case of a click, start at the mouse positiion
    //if this is being restored from XML, the next call to restore will override this with what is in the XML
    switchKey->timeRange.min = switchKey->timeRange.max = screenXToMillis(ofGetMouseX());
    switchKey->startSelected = false;
    switchKey->endSelected   = true; //true so you can drag the range to start with
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
}

void ofxTLSwitches::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //push the time range into X/Y
    ofxTLSwitch* switchKey = (ofxTLSwitch* )key;
    switchKey->time = switchKey->timeRange.min;
	xmlStore.addValue("max", timeline->getTimecode().timecodeForMillis(switchKey->timeRange.max));
}

ofxTLKeyframe* ofxTLSwitches::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
	for(int i = 0; i < keyframes.size(); i++){
		ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
    	if(switchKey->display.inside(p)){
            selectedIndex = i;
            return switchKey;
        }
    }
    selectedIndex = -1;
    return NULL;
}

string ofxTLSwitches::getTrackType(){
    return "Switches";
}