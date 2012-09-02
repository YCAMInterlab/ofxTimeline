//
//  ofxTLCurves.cpp
//  KeyframeSelection
//
//  Created by James George on 7/25/12.
//  Copyright (c) 2012 FlightPhase. All rights reserved.
//

#include "ofxTLCurves.h"
#include "ofxTimeline.h"

ofxTLCurves::ofxTLCurves(){
	initializeEasings();
	valueRange = ofRange(0.0, 1.0);
	drawingEasingWindow = false;
}

float ofxTLCurves::interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long sampleTime){
	ofxTLTweenKeyframe* tweenKeyStart = (ofxTLTweenKeyframe*)start;
	ofxTLTweenKeyframe* tweenKeyEnd = (ofxTLTweenKeyframe*)end;
	return ofxTween::map(sampleTime, tweenKeyStart->time, tweenKeyEnd->time,
						 			 tweenKeyStart->value, tweenKeyEnd->value,
						 false, *tweenKeyStart->easeFunc->easing, tweenKeyStart->easeType->type);
}

string ofxTLCurves::getTrackType(){
	return "Curves";    
}

//void ofxTLCurves::draw(){
//	
//
//}

ofxTLKeyframe* ofxTLCurves::newKeyframe(){
	ofxTLTweenKeyframe* k = new ofxTLTweenKeyframe();
	k->easeFunc = easingFunctions[0];
	k->easeType = easingTypes[0];
	return k;
}

//void ofxTLCurves::playbackStarted(ofxTLPlaybackEventArgs& args){
//	
//}
//
//void ofxTLCurves::playbackLooped(ofxTLPlaybackEventArgs& args){
//	
//}
//
//void ofxTLCurves::playbackEnded(ofxTLPlaybackEventArgs& args){
//	
//}

void ofxTLCurves::drawModalContent(){
	
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

void ofxTLCurves::mousePressed(ofMouseEventArgs& args, long millis){

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
				shouldRecomputePreviews = true;
                return;
            }
        }
        
        for(int i = 0; i < easingTypes.size(); i++){
            if(easingTypes[i]->bounds.inside(screenpoint-easingWindowPosition)){
                for(int k = 0; k < selectedKeyframes.size(); k++){
                	((ofxTLTweenKeyframe*)selectedKeyframes[k])->easeType = easingTypes[i];
                }
                timeline->flagTrackModified(this);
				shouldRecomputePreviews = true;
                return;
            }
        }
    }
    else{
        ofxTLKeyframes::mousePressed(args, millis);
    }
}

void ofxTLCurves::mouseDragged(ofMouseEventArgs& args, long millis){
	if(!drawingEasingWindow){
        ofxTLKeyframes::mouseDragged(args, millis);
    }
}

void ofxTLCurves::selectedKeySecondaryClick(ofMouseEventArgs& args){
    easingWindowPosition = ofVec2f(MIN(args.x, bounds.width - easingBoxWidth),
                                   MIN(args.y, (timeline->getDrawRect().y + timeline->getDrawRect().height) - (easingBoxHeight*easingFunctions.size() + easingBoxHeight*easingTypes.size())));
    
    drawingEasingWindow = true;
    timeline->presentedModalContent(this);
}

void ofxTLCurves::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTweenKeyframe* tweenKey =  (ofxTLTweenKeyframe*)key;    
    tweenKey->easeFunc = easingFunctions[ofClamp(xmlStore.getValue("easefunc", 0), 0, easingFunctions.size()-1)];
    tweenKey->easeType = easingTypes[ofClamp(xmlStore.getValue("easetype", 0), 0, easingTypes.size()-1)];
}

void ofxTLCurves::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTweenKeyframe* tweenKey =  (ofxTLTweenKeyframe*)key;
    xmlStore.addValue("easefunc", tweenKey->easeFunc->id);
    xmlStore.addValue("easetype", tweenKey->easeType->id);
}

void ofxTLCurves::initializeEasings(){
    
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

