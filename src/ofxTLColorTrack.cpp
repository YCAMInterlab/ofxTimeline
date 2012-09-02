//
//  ofxTLColorTrack.cpp
//  Duration
//
//  Created by Jim on 8/31/12.
//
//

#include "ofxTLColorTrack.h"
#include "ofxTimeline.h"

ofxTLColorTrack::ofxTLColorTrack()
 :	drawingColorWindow(false),
	defaultColor(ofColor(0,0,0))
{
	//
}

void ofxTLColorTrack::draw(){
	if(viewIsDirty){
		updateColorMesh();
	}
}

void ofxTLColorTrack::drawModalContent(){
	
}

void ofxTLColorTrack::loadColorPalette(ofBaseHasPixels& image){
	colorPallete.setFromPixels(image.getPixelsRef());
}

void ofxTLColorTrack::loadColorPalette(string imagePath){
	colorPallete.loadImage(imagePath);
}

ofColor ofxTLColorTrack::getColor(){
	return getColorAtMillis(timeline->getCurrentTimeMillis());
}

ofColor ofxTLColorTrack::getColorAtSecond(float second){
	return getColorAtMillis(second*1000);
}

ofColor ofxTLColorTrack::getColorAtPosition(float pos){
	return getColorAtMillis(pos * timeline->getDurationInMilliseconds());
}

ofColor ofxTLColorTrack::getColorAtMillis(unsigned long millis){
	if(keyframes.size() == 0){
		return defaultColor;
	}
	
	if(millis < keyframes[0]->time){
		return defaultColor;
	}
	
	if(millis > keyframes[keyframes.size()-1]->time){
		return defaultColor;
	}
	
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->time >= millis){
			ofxTLColorSample* startSample = (ofxTLColorSample*)keyframes[i-1];
			ofxTLColorSample* endSample = (ofxTLColorSample*)keyframes[i];
			return startSample->sample.getLerped(endSample->sample,
												 ofMap(millis, startSample->time, endSample->time, 0, 1, true));

		}
	}
}

void ofxTLColorTrack::setDefaultColor(ofColor color){
	defaultColor = color;
}

ofColor ofxTLColorTrack::getDefaultColor(){
	return defaultColor;	
}


//For selecting keyframe type only,
//the superclass controls keyframe placement
bool ofxTLColorTrack::mousePressed(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		return true;
	}
	else{
		return ofxTLTrack::mousePressed(args, millis);
	}
}

void ofxTLColorTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		//
	}
	else{
		ofxTLTrack::mouseDragged(args, millis);
	}
}

void ofxTLColorTrack::updateColorMesh(){
	for(int i = 0; i < keyframes.size(); i++){
		ofxTLColorSample* sample = (ofxTLColorSample*)keyframes[i];
	}
}

void ofxTLColorTrack::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	
}

void ofxTLColorTrack::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	
}

void ofxTLColorTrack::selectedKeySecondaryClick(ofMouseEventArgs& args){
	
}

string ofxTLColorTrack::getTrackType(){
	return "Colors";
}
