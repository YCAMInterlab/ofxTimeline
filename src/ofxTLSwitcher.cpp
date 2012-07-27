
#include "ofxTLSwitcher.h"
#include "ofxTimeline.h"

ofxTLSwitcher::ofxTLSwitcher(){
    
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
        if(isKeyframeSelected(switchKey)){
        	ofSetColor(timeline->getColors().highlightColor, 100);    
        }
        else{
        	ofSetColor(timeline->getColors().keyColor, 100);
        }
        ofRect(switchKey->display);
    }
    ofPopStyle();
    
}

bool ofxTLSwitcher::isOn(float percent){
    for(int i = 0; i < keyframes.size(); i++){
    	
    }
    return false;
}

void ofxTLSwitcher::mouseDragged(ofMouseEventArgs& args, bool snapped){
    ofxTLKeyframer::mouseDragged(args, snapped);
	for(int i = 0; i < selectedKeyframes.size(); i++){
        ofxTLSwitch* switchKey =  (ofxTLSwitch*)selectedKeyframes[i];
        float dif = switchKey->position.x - switchKey->time.min;
        switchKey->time.min = switchKey->position.x;
        switchKey->time.max += dif;
    }
}

ofxTLKeyframe* ofxTLSwitcher::newKeyframe(ofVec2f point){
    ofxTLSwitch* switchKey = new ofxTLSwitch();
    switchKey->position = point; //Not used, but hacked for storage purposes
    switchKey->time.min = point.x;
    switchKey->time.max = point.x + getSmallRange();
    switchKey->startSelected = false;
    switchKey->endSelected   = true;
    return switchKey;
}

void ofxTLSwitcher::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //pull the saved x/y values into min/max a little tricky...
    ofxTLSwitch* switchKey = (ofxTLSwitch*)key;
    switchKey->time.min = switchKey->position.x;
	switchKey->time.max = xmlStore.getValue("max", switchKey->time.min);
}

void ofxTLSwitcher::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    //push the time range into X/Y
    ofxTLSwitch* switchKey = (ofxTLSwitch* )key;
    switchKey->position.x = switchKey->time.min;
	xmlStore.addValue("max", switchKey->time.max);
}

float ofxTLSwitcher::getSmallRange(){
    //just return 5% of the view as an arbr
 	return screenXtoNormalizedX(bounds.width*.05) - screenXtoNormalizedX(0);
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
