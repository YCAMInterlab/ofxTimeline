
#include "ofxTimeline.h"
#include "ofxTLBangs.h"

ofxTLBangs::ofxTLBangs(){
    lastTimelinePoint = 0;
	lastBangTime = 0;
}

ofxTLBangs::~ofxTLBangs(){
	disable();
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
    }
    ofPopStyle();

}

void ofxTLBangs::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
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
	if(isPlaying || timeline->getIsPlaying()){
		long thisTimelinePoint = currentTrackTime();
		for(int i = 0; i < keyframes.size(); i++){
			if(timeline->getInOutRangeMillis().contains(keyframes[i]->time) && lastTimelinePoint < keyframes[i]->time && thisTimelinePoint >= keyframes[i]->time){
//				ofLogNotice() << "fired bang with accuracy of " << (keyframes[i]->time - thisTimelinePoint) << endl;
				bangFired(keyframes[i]);
				lastBangTime = ofGetElapsedTimef();
			}
		}
		lastTimelinePoint = thisTimelinePoint;
	}
}

void ofxTLBangs::bangFired(ofxTLKeyframe* key){
    ofxTLBangEventArgs args;
    args.sender = timeline;
    args.track = this;
	//play solo change
    //args.currentMillis = timeline->getCurrentTimeMillis();
	args.currentMillis = currentTrackTime();
    args.currentPercent = timeline->getPercentComplete();
    args.currentFrame = timeline->getCurrentFrame();
    args.currentTime = timeline->getCurrentTime();
    ofNotifyEvent(events().bangFired, args);    
}

void ofxTLBangs::playbackStarted(ofxTLPlaybackEventArgs& args){
	ofxTLTrack::playbackStarted(args);
	lastTimelinePoint = currentTrackTime();
}

void ofxTLBangs::playbackEnded(ofxTLPlaybackEventArgs& args){
//    isPlayingBack = false;
}

void ofxTLBangs::playbackLooped(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = 0;
}

string ofxTLBangs::getTrackType(){
    return "Bangs";
}
