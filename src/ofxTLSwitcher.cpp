
#include "ofxTLSwitcher.h"
#include "ofxTimeline.h"
#include "ofxTLUtils.h"

ofxTLSwitcher::ofxTLSwitcher(){
    edgeSelected = false;
}

ofxTLSwitcher::~ofxTLSwitcher(){
    
}

void ofxTLSwitcher::draw(){
    
    ofPushStyle();
	ofFill();
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        int startScreenX = normalizedXtoScreenX(switchKey->time.min);
        int endScreenX = normalizedXtoScreenX(switchKey->time.max);
		switchKey->display = ofRectangle(startScreenX, bounds.y, endScreenX-startScreenX, bounds.height);

        //draw handles

        ofSetLineWidth(2);
        bool keyIsSelected = isKeyframeSelected(switchKey);
        if(keyIsSelected || switchKey->startSelected){
	        ofSetColor(timeline->getColors().highlightColor);                
        }
        else{
	        ofSetColor(timeline->getColors().keyColor);    
        }        

        ofLine(switchKey->display.x, bounds.y, 
               switchKey->display.x, bounds.y+bounds.height);

        if(keyIsSelected || switchKey->endSelected){
	        ofSetColor(timeline->getColors().highlightColor);                
        }
        else{
	        ofSetColor(timeline->getColors().keyColor);    
        }        
        ofLine(switchKey->display.x+switchKey->display.width, bounds.y, 
               switchKey->display.x+switchKey->display.width, bounds.y+bounds.height);

        //draw region
        if(keyIsSelected){
        	ofSetColor(timeline->getColors().highlightColor, 100);    
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

bool ofxTLSwitcher::isOn(float percent){
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if(switchKey->time.min > percent){
            break;
        }
        if(switchKey->time.contains(percent)){
            return true;
        }
    }
    return false;
}

void ofxTLSwitcher::mousePressed(ofMouseEventArgs& args){    
    
    //check to see if we are close to any edges, if so select them
    bool startSelected = false;
    bool endSelected = false;
    for(int i = 0; i < keyframes.size(); i++){
        
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        startSelected = abs(switchKey->display.x - args.x) < 10.0;
        if (startSelected && !switchKey->startSelected && !ofGetModifierKeyShift()) {
            timeline->unselectAll();
        }
        switchKey->startSelected = startSelected;
        if(switchKey->startSelected){
            switchKey->edgeDragOffset = args.x - switchKey->display.x;
        }
        float endEdge = switchKey->display.x+switchKey->display.width;
        endSelected = abs(endEdge - args.x) < 10.0;
        //don't let them both be selected in one click!
        if(!startSelected && endSelected && !switchKey->endSelected && !ofGetModifierKeyShift()){
            timeline->unselectAll();
        }
        switchKey->endSelected = endSelected && !startSelected;
        if(switchKey->endSelected){
            switchKey->edgeDragOffset = args.x - endEdge;
        }
        
        if(startSelected || endSelected){
            break;
        }
        
    }
    
    if(!endSelected && !startSelected){
    	//normal selection from above
	    ofxTLKeyframer::mousePressed(args);
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

void ofxTLSwitcher::unselectAll(){
    ofxTLKeyframer::unselectAll();
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        switchKey->startSelected = switchKey->endSelected = false;
    }
}

void ofxTLSwitcher::mouseDragged(ofMouseEventArgs& args, bool snapped){
    
    //update to make sure that any keyframes with edges selected are properly added or removed 
    
    //do the normal dragging behavior for selected keyframes
    ofxTLKeyframer::mouseDragged(args, snapped);

    //the superclass will move the ->position.x value with the drag
    //so we look at the selected keyframes values and see if their changed
    //if so update both the min and the max time so the control moves as a block
	for(int i = 0; i < selectedKeyframes.size(); i++){
        ofxTLSwitch* switchKey =  (ofxTLSwitch*)selectedKeyframes[i];
        float dif = switchKey->position.x - switchKey->time.min;
        switchKey->time.min = switchKey->position.x;
        switchKey->time.max += dif;
    }
    
    //now look for any keys with just beginning and ends selected
    //becaues of the logical in the mousePressed, there will never
    //be a selected keyframe with
	for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        if(switchKey->startSelected){
            switchKey->time.min = screenXtoNormalizedX(args.x - switchKey->edgeDragOffset);
            switchKey->position.x = switchKey->time.min;
        }
        else if(switchKey->endSelected){
            switchKey->time.max = screenXtoNormalizedX(args.x - switchKey->edgeDragOffset);
        }
    }

    for(int i = 0; i < keyframes.size(); i++){
        //check to see if the user reversed the value and swap them really quick
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
		if(switchKey->time.min > switchKey->time.max){
            float tempPos = switchKey->time.max;
            switchKey->time.max = switchKey->time.min;
            switchKey->time.min = switchKey->position.x = tempPos;
            bool tempSelect = switchKey->startSelected;
            switchKey->startSelected = switchKey->endSelected;
            switchKey->endSelected = tempSelect;
        }
    }

    //TODO: no overlaps!!
}

void ofxTLSwitcher::mouseMoved(ofMouseEventArgs& args){
    
    endHover = startHover = false;
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
//        if(switchKey->display.x > args.x){
//            break;
//        }
        if(abs(switchKey->display.x - args.x) < 10.0){
            hoverKeyframe = switchKey;
            startHover = true;
            return;
        }
        float endEdge = switchKey->display.x+switchKey->display.width;
        if(abs(endEdge - args.x) < 10.0){
            hoverKeyframe = switchKey;
            endHover = true;
            return;
        }
    }
    ofxTLKeyframer::mouseMoved(args);
}

void ofxTLSwitcher::mouseReleased(ofMouseEventArgs& args){
	ofxTLKeyframer::mouseReleased(args);
}

void ofxTLSwitcher::regionSelected(ofRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLSwitch* switchKey = (ofxTLSwitch*)keyframes[i];
        cout << "range is " << timeRange << " this switch is " << switchKey->time << endl;
    	if(switchKey->time.intersects(timeRange)){
            cout << "	selecting keyframe!" << endl;
            selectKeyframe(switchKey);
        }
    }
}

ofxTLKeyframe* ofxTLSwitcher::newKeyframe(ofVec2f point){
    ofxTLSwitch* switchKey = new ofxTLSwitch();
    switchKey->position = point; //Not used, but hacked for storage purposes
    switchKey->time.min = point.x;
    switchKey->time.max = point.x;// + getSmallRange();
    switchKey->startSelected = false;
    switchKey->endSelected   = true; //true so you can drag the range to start with
    return switchKey;
}

void ofxTLSwitcher::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //pull the saved x/y values into min/max a little tricky...
    ofxTLSwitch* switchKey = (ofxTLSwitch*)key;
    switchKey->time.min = switchKey->position.x;
	switchKey->time.max = xmlStore.getValue("max", switchKey->time.min);
    //this is so freshly restored keys won't have ends selected but click keys will
    switchKey->startSelected = switchKey->endSelected = false; 
}

void ofxTLSwitcher::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //push the time range into X/Y
    ofxTLSwitch* switchKey = (ofxTLSwitch* )key;
    switchKey->position.x = switchKey->time.min;
	xmlStore.addValue("max", switchKey->time.max);
}

float ofxTLSwitcher::getSmallRange(){
    //just return 2% of the view as an arbr
 	return screenXtoNormalizedX(bounds.width*.01) - screenXtoNormalizedX(0);
}

ofxTLKeyframe* ofxTLSwitcher::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
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
