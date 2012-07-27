
#include "ofxTimeline.h"
#include "ofxTLBangs.h"

ofxTLBangs::ofxTLBangs(){
    lastTimelinePoint = 0;
}

ofxTLBangs::~ofxTLBangs(){
    
}

void ofxTLBangs::enable(){
    ofxTLKeyframer::enable();
	ofxTLRegisterPlaybackEvents(this);    
}

void ofxTLBangs::disable(){
    ofxTLKeyframer::disable();
    ofxTLRemovePlaybackEvents(this);
}

void ofxTLBangs::draw(){
        
    if(bounds.height < 2){
        return;
    }
    
    ofPushStyle();
    ofFill();
    ofSetLineWidth(5);
    for(int i = keyframes.size()-1; i >= 0; i--){
        int screenX = normalizedXtoScreenX(keyframes[i]->position.x, zoomBounds);
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

ofxTLKeyframe* ofxTLBangs::keyframeAtScreenpoint(ofVec2f p, int& selectedIndex){
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

void ofxTLBangs::update(ofEventArgs& args){
	float thisTimelinePoint = timeline->getPercentComplete();
	for(int i = 0; i < keyframes.size(); i++){
		if(lastTimelinePoint < keyframes[i]->position.x && thisTimelinePoint > keyframes[i]->position.x){
			bangFired(keyframes[i]);
        }
	}
	lastTimelinePoint = thisTimelinePoint;
}

void ofxTLBangs::bangFired(ofxTLKeyframe* key){
    ofxTLBangEventArgs args;
    args.trackName = name;
    ofNotifyEvent(ofxTLEvents.bangFired, args);    
}

void ofxTLBangs::playbackStarted(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = timeline->getPercentComplete();
	ofAddListener(ofEvents().update, this, &ofxTLBangs::update);
}

void ofxTLBangs::playbackEnded(ofxTLPlaybackEventArgs& args){
	ofRemoveListener(ofEvents().update, this, &ofxTLBangs::update);
}

void ofxTLBangs::playbackLooped(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = 0;
}
