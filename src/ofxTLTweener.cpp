//
//  ofxTLTweener.cpp
//  KeyframeSelection
//
//  Created by James George on 7/25/12.
//  Copyright (c) 2012 FlightPhase. All rights reserved.
//

#include "ofxTLTweener.h"
#include "ofxTimeline.h"

ofxTLTweener::ofxTLTweener(){
	initializeEasings();
	valueRange = ofRange(0.0, 1.0);
	drawingEasingWindow = false;
}

//TODO potentially scale internal values at this point
void ofxTLTweener::setValueRange(ofRange range, float newDefaultValue){
	valueRange = range;
    defaultValue = newDefaultValue;
}

ofRange ofxTLTweener::getValueRange(){
	return valueRange;    
}

//main function to get values out of the timeline, operates on the given value range
float ofxTLTweener::getValueAtPercent(float percent){
//	return ofMap(sampleAt(percent), 0.0, 1.0, valueRange.min, valueRange.max, false);
    return getValueAtTimeInMillis(percent*timeline->getDurationInMilliseconds());
}

float ofxTLTweener::getValueAtTimeInMillis(long sampleTime){
	return ofMap(sampleAtTime(sampleTime), 0.0, 1.0, valueRange.min, valueRange.max, false);
}

float ofxTLTweener::sampleAtPercent(float percent){
	return sampleAtTime(percent * timeline->getDurationInMilliseconds());
}

float ofxTLTweener::sampleAtTime(long sampleTime){
	sampleTime = ofClamp(sampleTime, 0, timeline->getDurationInMilliseconds());
	
	//edge cases
	if(keyframes.size() == 0){
		return ofMap(defaultValue, valueRange.min, valueRange.max, 0, 1.0, true);
	}
	
	if(sampleTime < keyframes[0]->time){
		return keyframes[0]->value;
	}
	
	if(sampleTime > keyframes[keyframes.size()-1]->time){
		return keyframes[keyframes.size()-1]->value;
	}
	
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->time >= sampleTime){
            ofxTLTweenKeyframe* tweenKey = (ofxTLTweenKeyframe*)keyframes[i-1];
			float percentBetween = ofxTween::map(sampleTime, tweenKey->time, keyframes[i]->time, 0.0, 1.0, false, *tweenKey->easeFunc->easing, tweenKey->easeType->type);
			return tweenKey->value * (1.-percentBetween) + keyframes[i]->value*percentBetween;
		}
	}
	
	ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- Error condition, couldn't find keyframe for percent " + ofToString(sampleTime));
	return defaultValue;
}

string ofxTLTweener::getTrackType(){
	return "Tweens";    
}

void ofxTLTweener::draw(){
	
	if(bounds.width == 0 || bounds.height == 0){
		return;
	}
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableSmoothing();

	// DRAW KEYFRAME LINES

	ofSetColor(timeline->getColors().keyColor);
	ofNoFill();
	ofBeginShape();
	if(keyframes.size() == 0 || keyframes.size() == 1){
		ofVertex(bounds.x, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height);
		ofVertex(bounds.x+bounds.width, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height);
	}
	else{
		for(int p = bounds.x; p <= bounds.width; p++){
            //TODO: cache this into a poly line to avoid insane sampling.
			ofVertex(p,  bounds.y + bounds.height - sampleAtPercent(screenXtoNormalizedX(p)) * bounds.height);
		}
	}
	ofEndShape(false);

    
	//**** DRAW KEYFRAME DOTS
	ofSetColor(timeline->getColors().keyColor);
	for(int i = 0; i < keyframes.size(); i++){
		if(!isKeyframeIsInBounds(keyframes[i])){
			continue;
		}
		
  		ofVec2f screenpoint = screenPositionForKeyframe(keyframes[i]);		
		if(keyframes[i] == hoverKeyframe){
			ofPushStyle();
			ofFill();
			ofSetColor(timeline->getColors().highlightColor);
			ofCircle(screenpoint.x, screenpoint.y, 6);
			ofPopStyle();
		}
        

		if(isKeyframeSelected( keyframes[i] )){
            ofSetColor(timeline->getColors().textColor);
			float keysValue = ofMap(keyframes[i]->value, 0, 1.0, valueRange.min, valueRange.max, true);
			string frameString = timeline->formatTime(keyframes[i]->time);
            if(keysAreDraggable){
				ofDrawBitmapString(ofToString(keysValue, 4), screenpoint.x+5, screenpoint.y-5);
            }
			ofFill();
            ofCircle(screenpoint.x, screenpoint.y, 4);
		}
		else{
            ofSetColor(timeline->getColors().textColor);
			ofNoFill();
            ofCircle(screenpoint.x, screenpoint.y, 2);
		}
		

	}
	
	ofPopMatrix();
	ofPopStyle();
}


ofxTLKeyframe* ofxTLTweener::newKeyframe(){
	ofxTLTweenKeyframe* k = new ofxTLTweenKeyframe();
	k->easeFunc = easingFunctions[0];
	k->easeType = easingTypes[0];
	return k;
}

void ofxTLTweener::drawModalContent(){
	
	//****** DRAW EASING CONTROLS
	if(!drawingEasingWindow){
    	return;
    }
    
    for(int i = 0; i < easingFunctions.size(); i++){
        //TODO turn into something like selectionContainsEaseFunc();
        if(easingFunctions[i] == ((ofxTLTweenKeyframe*)selectedKeyframes[0])->easeFunc){
            ofSetColor(150, 100, 10);
        }
        else{
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofRect(easingWindowPosition.x + easingFunctions[i]->bounds.x, easingWindowPosition.y +easingFunctions[i]->bounds.y, 
               easingFunctions[i]->bounds.width, easingFunctions[i]->bounds.height);
        ofSetColor(200, 200, 200);
        ofDrawBitmapString(easingFunctions[i]->name, 
                           easingWindowPosition.x + easingFunctions[i]->bounds.x+10, 
                           easingWindowPosition.y + easingFunctions[i]->bounds.y+15);			
        
        ofNoFill();
        ofSetColor(40, 40, 40);
        ofRect(easingWindowPosition.x + easingFunctions[i]->bounds.x, easingWindowPosition.y +easingFunctions[i]->bounds.y, 
               easingFunctions[i]->bounds.width, easingFunctions[i]->bounds.height);	
    }
    
    for(int i = 0; i < easingTypes.size(); i++){
        //TODO turn into something like selectionContainsEaseType();
        //so that we can show the multi-selected easies
        if(easingTypes[i] ==  ((ofxTLTweenKeyframe*)selectedKeyframes[0])->easeType){
            ofSetColor(150, 100, 10);
        }
        else{
            ofSetColor(80, 80, 80);
        }
        ofFill();
        ofRect(easingWindowPosition.x + easingTypes[i]->bounds.x, easingWindowPosition.y + easingTypes[i]->bounds.y, 
               easingTypes[i]->bounds.width, easingTypes[i]->bounds.height);
        ofSetColor(200, 200, 200);
        ofDrawBitmapString(easingTypes[i]->name, 
                           easingWindowPosition.x + easingTypes[i]->bounds.x+10, 
                           easingWindowPosition.y + easingTypes[i]->bounds.y+15);			
        ofNoFill();
        ofSetColor(40, 40, 40);
        ofRect(easingWindowPosition.x + easingTypes[i]->bounds.x, 
               easingWindowPosition.y + easingTypes[i]->bounds.y, 
               easingTypes[i]->bounds.width, easingTypes[i]->bounds.height);
    }
}

void ofxTLTweener::mousePressed(ofMouseEventArgs& args, long millis){

    //TODO: move this to mouse-released to cooperate with UNDO
    if(drawingEasingWindow){
        //see if we clicked on an
        drawingEasingWindow = false;
        timeline->dismissedModalContent();
        ofVec2f screenpoint(args.x,args.y);
        for(int i = 0; i < easingFunctions.size(); i++){
            if(easingFunctions[i]->bounds.inside(screenpoint-easingWindowPosition)){
                for(int k = 0; k < selectedKeyframes.size(); k++){                    
                    ((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeFunc = easingFunctions[i];
                }
                timeline->flagTrackModified(this); 
                return;
            }
        }
        
        for(int i = 0; i < easingTypes.size(); i++){
            if(easingTypes[i]->bounds.inside(screenpoint-easingWindowPosition)){
                for(int k = 0; k < selectedKeyframes.size(); k++){
                	((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeType = easingTypes[i];
                }
                timeline->flagTrackModified(this);
                return;
            }
        }
    }
    else{
        ofxTLKeyframer::mousePressed(args, millis);
    }
}

void ofxTLTweener::mouseDragged(ofMouseEventArgs& args, long millis){
	if(!drawingEasingWindow){
        ofxTLKeyframer::mouseDragged(args, millis);
    }
}

void ofxTLTweener::selectedKeySecondaryClick(ofMouseEventArgs& args){
    easingWindowPosition = ofVec2f(MIN(args.x, bounds.width - easingBoxWidth),
                                   MIN(args.y, (timeline->getDrawRect().y + timeline->getDrawRect().height) - (easingBoxHeight*easingFunctions.size() + easingBoxHeight*easingTypes.size())));
    
    drawingEasingWindow = true;
    timeline->presentedModalContent(this);
}

void ofxTLTweener::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTweenKeyframe* tweenKey =  (ofxTLTweenKeyframe*)key;    
    tweenKey->easeFunc = easingFunctions[ofClamp(xmlStore.getValue("easefunc", 0), 0, easingFunctions.size()-1)];
    tweenKey->easeType = easingTypes[ofClamp(xmlStore.getValue("easetype", 0), 0, easingTypes.size()-1)];
}

void ofxTLTweener::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTweenKeyframe* tweenKey =  (ofxTLTweenKeyframe*)key;
    xmlStore.addValue("easefunc", tweenKey->easeFunc->id);
    xmlStore.addValue("easetype", tweenKey->easeType->id);
}

void ofxTLTweener::initializeEasings(){
    
	//FUNCTIONS ----
	EasingFunction* ef;
	ef = new EasingFunction();
	ef->easing = new ofxEasingLinear();
	ef->name = "linear";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingSine();
	ef->name = "sine";
	easingFunctions.push_back(ef);
    
	ef = new EasingFunction();
	ef->easing = new ofxEasingCirc();
	ef->name = "circular";
	easingFunctions.push_back(ef);
    
	ef = new EasingFunction();
	ef->easing = new ofxEasingQuad();
	ef->name = "quadratic";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingCubic();
	ef->name = "cubic";
	easingFunctions.push_back(ef);
    
	ef = new EasingFunction();
	ef->easing = new ofxEasingQuart();
	ef->name = "quartic";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingQuint();
	ef->name = "quintic";
	easingFunctions.push_back(ef);
    
	ef = new EasingFunction();
	ef->easing = new ofxEasingExpo();
	ef->name = "exponential";
	easingFunctions.push_back(ef);
	
	ef = new EasingFunction();
	ef->easing = new ofxEasingBack();
	ef->name = "back";
	easingFunctions.push_back(ef);
    
	ef = new EasingFunction();
	ef->easing = new ofxEasingBounce();
	ef->name = "bounce";
	easingFunctions.push_back(ef);
    
	ef = new EasingFunction();
	ef->easing = new ofxEasingElastic();
	ef->name = "elastic";
	easingFunctions.push_back(ef);
    
	///TYPES -------
	EasingType* et;
	et = new EasingType();
	et->type = ofxTween::easeIn;
	et->name = "ease in";
	easingTypes.push_back(et);
    
	et = new EasingType();
	et->type = ofxTween::easeOut;
	et->name = "ease out";
	easingTypes.push_back(et);
    
	et = new EasingType();
	et->type = ofxTween::easeInOut;
	et->name = "ease in-out";
	easingTypes.push_back(et);
	
	//TODO: make configurable
	easingBoxWidth  = 120;
	easingBoxHeight = 20;
	easingWindowSeperatorHeight = 4;
	
	for(int i = 0; i < easingFunctions.size(); i++){
		easingFunctions[i]->bounds = ofRectangle(0, i*easingBoxHeight, easingBoxWidth, easingBoxHeight);
		easingFunctions[i]->id = i;
	}
	
	for(int i = 0; i < easingTypes.size(); i++){
		easingTypes[i]->bounds = ofRectangle(0, (i+easingFunctions.size())*easingBoxHeight + easingWindowSeperatorHeight, easingBoxWidth, easingBoxHeight);
		easingTypes[i]->id = i;
	}
}

