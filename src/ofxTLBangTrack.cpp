
#include "ofxTimeline.h"
#include "ofxTLBangTrack.h"

ofxTLBangTrack::ofxTLBangTrack(){
    lastTimelinePoint = 0;
}

ofxTLBangTrack::~ofxTLBangTrack(){
    
}

void ofxTLBangTrack::enable(){
	ofxTLRegisterPlaybackEvents(this);    
}

void ofxTLBangTrack::disable(){
    ofxTLRemovePlaybackEvents(this);
}

void ofxTLBangTrack::draw(){
        
    if(bounds.height < 2){
        return;
    }
    
    //TODO: Super class general drawing!!
    ofPushStyle();
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
    
    ofFill();
    ofSetLineWidth(5);
    for(int i = keyframes.size()-1; i >= 0; i--){
        int screenX = normalizedXtoScreenX(keyframes[i]->position.x, zoomBounds);
//        if(&triggers[i] == selectedTrigger){
        if(isKeyframeSelected(keyframes[i])){
            ofSetColor(timeline->getColors().textColor);
        }
        else if(keyframes[i] == hoverKeyframe){
            ofSetColor(timeline->getColors().highlightColor);
        }
        else{
            ofSetColor(timeline->getColors().keyColor);
        }
        
        int textHeight = bounds.y + 10 + ( (20*i) % int(bounds.height) );
        ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
        ofSetColor(timeline->getColors().backgroundColor);                
    }
    ofPopStyle();

}

ofxTLKeyframe* ofxTLBangTrack::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
	for(int i = 0; i < keyframes.size(); i++){
		float offset = p.x - normalizedXtoScreenX(keyframes[i]->position.x, zoomBounds);
		if (abs(offset) < 5) {
            selectedIndex = i;
			return keyframes[i];
		}
	}
    selectedIndex = -1;
	return NULL;    
}

void ofxTLBangTrack::update(ofEventArgs& args){
	float thisTimelinePoint = timeline->getPercentComplete();
	for(int i = 0; i < keyframes.size(); i++){
		if(lastTimelinePoint < keyframes[i]->position.x && thisTimelinePoint > keyframes[i]->position.x){
			ofxTLTriggerEventArgs args;
//			args.triggerName = triggers[i].name;
//			args.triggerGroupName = name;
			ofNotifyEvent(ofxTLEvents.trigger, args);
		}
	}
	lastTimelinePoint = thisTimelinePoint;
    
}

void ofxTLBangTrack::playbackStarted(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = timeline->getPercentComplete();
	ofAddListener(ofEvents().update, this, &ofxTLBangTrack::update);
}

void ofxTLBangTrack::playbackEnded(ofxTLPlaybackEventArgs& args){
	ofRemoveListener(ofEvents().update, this, &ofxTLBangTrack::update);
}

void ofxTLBangTrack::playbackLooped(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = 0;
}

