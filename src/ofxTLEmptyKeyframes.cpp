/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ofxTLEmptyKeyframes.h"
#include "ofxTimeline.h"

ofxTLEmptyKeyframes::ofxTLEmptyKeyframes(){
	
}

ofxTLEmptyKeyframes::~ofxTLEmptyKeyframes(){
	
}

//draw your keyframes into bounds
void ofxTLEmptyKeyframes::draw(){
	
	ofPushStyle();
	
	ofFill();
	//show the current color as background based on the playhead position
	ofSetColor(getCurrentColor(), 100);
	ofRect(bounds);

	for(int i = 0; i < keyframes.size(); i++){
		//make sure it's on screen
		if(isKeyframeIsInBounds(keyframes[i])){
			//we know the type because we created it in newKeyframe()
			//so we can safely cast
			ofxTLEmptyKeyframe* emptyKeyframe = (ofxTLEmptyKeyframe*)keyframes[i];
			if(hoverKeyframe == emptyKeyframe){
				ofSetColor(timeline->getColors().highlightColor);
			}
			else if(isKeyframeSelected(emptyKeyframe)){
				ofSetColor(timeline->getColors().textColor);
			}
			else{
				ofSetColor(timeline->getColors().keyColor);
			}
			ofVec2f screenPoint = screenPositionForKeyframe(emptyKeyframe);
			ofCircle(screenPoint, 7);
			ofSetColor(emptyKeyframe->color);
			ofCircle(screenPoint, 5);
		}
	}
	
	ofPopStyle();
}

ofColor ofxTLEmptyKeyframes::getCurrentColor(){
	return getColorAtTime(timeline->getCurrentTimeMillis());
}

ofColor ofxTLEmptyKeyframes::getColorAtTime(unsigned long long sampleTime){
	
	//return black if there are no frames
	if(keyframes.size() == 0){
		return ofColor(0,0,0);
	}
	//just one, or sampling before the first we can just return the first
	if(keyframes.size() == 1 || keyframes[0]->time >= sampleTime){
		return ((ofxTLEmptyKeyframe*)keyframes[0])->color;
	}
	//sampling after the last we return the last
	if(keyframes[keyframes.size()-1]->time <= sampleTime){
		return ((ofxTLEmptyKeyframe*)keyframes[keyframes.size()-1])->color;
	}
	
	//now we are somewhere in between, search
	//keyframes will always be sorted
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->time >= sampleTime){
			ofxTLEmptyKeyframe* prevKey  = (ofxTLEmptyKeyframe*)keyframes[i-1];
			ofxTLEmptyKeyframe* nextKey  = (ofxTLEmptyKeyframe*)keyframes[i];
			//interpolate
			float alpha = ofMap(sampleTime, prevKey->time, nextKey->time, 0, 1.0);
			return prevKey->color.getLerped(nextKey->color, alpha);
		}
	}
}

bool ofxTLEmptyKeyframes::mousePressed(ofMouseEventArgs& args, long millis){
	//for the general behavior call the super class
	//or you can do your own thing. Return true if the click caused an item to
	//become selectd
	return ofxTLKeyframes::mousePressed(args, millis);
}

void ofxTLEmptyKeyframes::mouseMoved(ofMouseEventArgs& args, long millis){
	ofxTLKeyframes::mouseMoved(args, millis);
}

void ofxTLEmptyKeyframes::mouseDragged(ofMouseEventArgs& args, long millis){
	ofxTLKeyframes::mouseDragged(args, millis);
}

void ofxTLEmptyKeyframes::mouseReleased(ofMouseEventArgs& args, long millis){
	ofxTLKeyframes::mouseReleased(args, millis);
}

//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
void ofxTLEmptyKeyframes::keyPressed(ofKeyEventArgs& args){
	ofxTLKeyframes::keyPressed(args);
}

void ofxTLEmptyKeyframes::regionSelected(ofLongRange timeRange, ofRange valueRange){
	//you can override the default to select other things than just dots
	ofxTLKeyframes::regionSelected(timeRange, valueRange);
}

string ofxTLEmptyKeyframes::getTrackType(){
	return "EmptyKeyframes";
}

ofxTLKeyframe* ofxTLEmptyKeyframes::newKeyframe(){
	//return our type of keyframe, stored in the parent class
	ofxTLEmptyKeyframe* newKey = new ofxTLEmptyKeyframe();
	newKey->color = ofColor(ofRandom(255),ofRandom(255),ofRandom(255));
	return newKey;
}

void ofxTLEmptyKeyframes::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLEmptyKeyframe* emptyKey = (ofxTLEmptyKeyframe*)key;
	emptyKey->color = ofColor(xmlStore.getValue("r", 255),
							  xmlStore.getValue("g", 255),
							  xmlStore.getValue("b", 255));
}

void ofxTLEmptyKeyframes::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLEmptyKeyframe* emptyKey = (ofxTLEmptyKeyframe*)key;
	xmlStore.addValue("r",emptyKey->color.r);
	xmlStore.addValue("g",emptyKey->color.g);
	xmlStore.addValue("b",emptyKey->color.b);
}

ofxTLKeyframe* ofxTLEmptyKeyframes::keyframeAtScreenpoint(ofVec2f p){
	return ofxTLKeyframes::keyframeAtScreenpoint(p);
}

void ofxTLEmptyKeyframes::selectedKeySecondaryClick(ofMouseEventArgs& args){
	//you can make a popup window start here
//	timeline->presentedModalContent(this);
	//and then when you want to get rid of it call somewhere else
//	timeline->dismissedModalContent();
	//this will lock all other timeline interaction and feed all things into your track
	//so feel free to draw out of bounds, but don't go off the screen or out of the timeline
}


void ofxTLEmptyKeyframes::willDeleteKeyframe(ofxTLKeyframe* keyframe){
	//do any cleanup before this keyframe gets hosed
}
