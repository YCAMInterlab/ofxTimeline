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

ofxTLTweener::~ofxTLTweener(){
    
}

void ofxTLTweener::setValueRange(ofRange range, float newDefaultValue){
	valueRange = range;
    defaultValue = newDefaultValue;
}

//main function to get values out of the timeline, operates on the given value range
float ofxTLTweener::getValueAtPercent(float percent){
	return ofMap(sampleAt(percent), 0.0, 1.0, valueRange.min, valueRange.max, false);
}

float ofxTLTweener::sampleAt(float percent){
	percent = ofClamp(percent, 0, 1.0);
	
	//edge cases
	if(keyframes.size() == 0){
		return ofMap(defaultValue, valueRange.min, valueRange.max, 0, 1.0, true);
	}
	
	if(percent < keyframes[0]->position.x){
		return keyframes[0]->position.y;
	}
	
	if(percent > keyframes[keyframes.size()-1]->position.x){
		return keyframes[keyframes.size()-1]->position.y;
	}
	
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->position.x >= percent){
            ofxTLTweenKeyframe* tweenKey = (ofxTLTweenKeyframe*)keyframes[i-1];
			float percentBetween = ofxTween::map(percent, tweenKey->position.x, keyframes[i]->position.x, 0.0, 1.0, false, *tweenKey->easeFunc->easing, tweenKey->easeType->type);
			return tweenKey->position.y * (1.-percentBetween) + keyframes[i]->position.y*percentBetween;
		}
	}
	
	ofLog(OF_LOG_ERROR, "ofxTLKeyframer --- Error condition, couldn't find keyframe for percent " + ofToString(percent, 4));
	return defaultValue;
}

void ofxTLTweener::draw(){
	
	if(bounds.width == 0 || bounds.height == 0){
		return;
	}
	
	ofPushStyle();
	ofPushMatrix();
	ofEnableSmoothing();

	// DRAW KEYFRAME LINES
	ofSetColor(timeline->getColors().highlightColor);
	ofNoFill();
	ofBeginShape();
	if(keyframes.size() == 0 || keyframes.size() == 1){
		ofVertex(bounds.x, bounds.y + bounds.height - sampleAt(.5)*bounds.height);
		ofVertex(bounds.x+bounds.width, bounds.y + bounds.height - sampleAt(.5)*bounds.height);
	}
	else{
		for(int p = 0; p <= bounds.width; p++){
            //TODO: cache this into a poly line to avoid insane sampling.
			ofVertex(bounds.x + p,  bounds.y + bounds.height - sampleAt(ofMap(p, 0, bounds.width, zoomBounds.min, zoomBounds.max, true)) * bounds.height);
		}
	}
	ofEndShape(false);

    
	//**** DRAW KEYFRAME DOTS
	ofSetColor(timeline->getColors().textColor);
	for(int i = 0; i < keyframes.size(); i++){
		if(!isKeyframeIsInBounds(keyframes[i])){
			continue;
		}
		
		ofSetColor(timeline->getColors().textColor);
		ofVec2f screenpoint = coordForKeyframePoint(keyframes[i]->position);
        
		if(isKeyframeSelected( keyframes[i] )){
			float keysValue = ofMap(keyframes[i]->position.y, 0, 1.0, valueRange.min, valueRange.max, true);
			string frameString = timeline->getIsFrameBased() ? 
            ofToString(int(keyframes[i]->position.x*timeline->getDurationInFrames())) : 
            timeline->formatTime(keyframes[i]->position.x*timeline->getDurationInSeconds());
            if(keysAreDraggable){
				ofDrawBitmapString(ofToString(keysValue, 4), screenpoint.x+5, screenpoint.y-5);
            }
			ofFill();
		}
		else{
			ofNoFill();
		}
		
		if(keyframes[i] == hoverKeyframe){
			ofPushStyle();
			ofFill();			
			ofSetColor(timeline->getColors().highlightColor);
			ofCircle(screenpoint.x, screenpoint.y, 8);
			ofPopStyle();
		}
		
		ofCircle(screenpoint.x, screenpoint.y, 4);
	}
	
	ofPopMatrix();
	ofPopStyle();
}

ofxTLKeyframe* ofxTLTweener::newKeyframe(ofVec2f point){
	ofxTLTweenKeyframe* k = new ofxTLTweenKeyframe();
	k->position = point;
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

void ofxTLTweener::mousePressed(ofMouseEventArgs& args){

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
        ofxTLKeyframer::mousePressed(args);
    }
}

void ofxTLTweener::mouseDragged(ofMouseEventArgs& args, bool snapped){
	if(!drawingEasingWindow){
        ofxTLKeyframer::mouseDragged(args, snapped);
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

