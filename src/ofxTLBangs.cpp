
#include "ofxTimeline.h"
#include "ofxTLBangs.h"

ofxTLBangs::ofxTLBangs(){
    lastTimelinePoint = 0;
    isPlayingBack = false;
	lastBangTime = 0;
}

ofxTLBangs::~ofxTLBangs(){
	disable();
}

void ofxTLBangs::enable(){
    if(!isEnabled()){
	    ofxTLKeyframes::enable();
		events().registerPlaybackEvents(this);    
    }
}

void ofxTLBangs::disable(){
    if(isEnabled()){
        if (isPlayingBack) {
//        	ofRemoveListener(ofEvents().update, this, &ofxTLBangs::update);
        }
	    ofxTLKeyframes::disable();
    	events().removePlaybackEvents(this);
    }
}

void ofxTLBangs::draw(){
        
    if(bounds.height < 2){
        return;
    }
    
    ofPushStyle();
    ofFill();
	
	//float currentPercent = powf(MIN(ofGetElapsedTimef() - lastBangTime, .5), 2);
	float currentPercent = powf(ofMap(ofGetElapsedTimef() - lastBangTime, 0, .5, 1.0, 0,true), 2);
	if(currentPercent > 0){
		ofSetColor(timeline->getColors().disabledColor, 100*(currentPercent));
		ofFill();
		ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	}
	
    for(int i = keyframes.size()-1; i >= 0; i--){
        //int screenX = normalizedXtoScreenX(keyframes[i]->position.x);
        int screenX = millisToScreenX(keyframes[i]->time);
        if(isKeyframeSelected(keyframes[i])){
            ofSetLineWidth(2);
            ofSetColor(timeline->getColors().textColor);
        }
        else if(keyframes[i] == hoverKeyframe){
            ofSetLineWidth(4);
            ofSetColor(timeline->getColors().highlightColor);
        }
        else{
            ofSetLineWidth(4);
            ofSetColor(timeline->getColors().keyColor);
        }
        
        ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
        //debug text system
//        ofSetColor(timeline->getColors().keyColor);                
//        int textHeight = bounds.y + 10 + ( (20*i) % int(bounds.height) );
//        ofDrawBitmapString(timeline->formatTime(keyframes[i]->time), screenX, textHeight);
    }
    ofPopStyle();

}

void ofxTLBangs::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
//        if(timeRange.contains( keyframes[i]->position.x)){
    	if(timeRange.contains( keyframes[i]->time )){
            selectKeyframe(keyframes[i]);
        }
	}
}

ofxTLKeyframe* ofxTLBangs::keyframeAtScreenpoint(ofVec2f p){
    if(bounds.inside(p.x, p.y)){
        for(int i = 0; i < keyframes.size(); i++){
            float offset = p.x - timeline->millisToScreenX(keyframes[i]->time);            
            if (abs(offset) < 5) {
                return keyframes[i];
            }
        }
    }
	return NULL;    
}

void ofxTLBangs::update(){
	if(isPlayingBack){
		long thisTimelinePoint = timeline->getCurrentTimeMillis();
		for(int i = 0; i < keyframes.size(); i++){
			if(lastTimelinePoint < keyframes[i]->time && thisTimelinePoint >= keyframes[i]->time){
//				ofLogNotice() << "fired bang with accuracy of " << (keyframes[i]->time - thisTimelinePoint) << endl;
				bangFired(keyframes[i]);
				lastBangTime = ofGetElapsedTimef();
			}
		}
		lastTimelinePoint = thisTimelinePoint;
	}
}

void ofxTLBangs::update(ofEventArgs& args){
	update();
}

void ofxTLBangs::bangFired(ofxTLKeyframe* key){
    ofxTLBangEventArgs args;
    args.sender = timeline;
    args.track = this;
    args.currentMillis = timeline->getCurrentTimeMillis();
    args.currentPercent = timeline->getPercentComplete();
    args.currentFrame = timeline->getCurrentFrame();
    args.currentTime = timeline->getCurrentTime();
    ofNotifyEvent(events().bangFired, args);    
}

void ofxTLBangs::playbackStarted(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = timeline->getCurrentTimeMillis();
//	ofAddListener(ofEvents().update, this, &ofxTLBangs::update);
    isPlayingBack = true;
}

void ofxTLBangs::playbackEnded(ofxTLPlaybackEventArgs& args){
//	ofRemoveListener(ofEvents().update, this, &ofxTLBangs::update);
    isPlayingBack = true;
}

void ofxTLBangs::playbackLooped(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = 0;
}

string ofxTLBangs::getTrackType(){
    return "Bangs";
}
