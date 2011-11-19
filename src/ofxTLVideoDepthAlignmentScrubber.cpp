/*
 *  ofxTLVideoDepthAlignmentScrubber.cpp
 *  RGBDPostAlign
 *
 *  Created by James George on 11/16/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLVideoDepthAlignmentScrubber.h"
bool pairsort(VideoDepthPair frameA, VideoDepthPair frameB){
	return frameA.videoFrame < frameB.videoFrame;
}

ofxTLVideoDepthAlignmentScrubber::ofxTLVideoDepthAlignmentScrubber(){
	videoSequence = NULL;
	depthSequence = NULL;
}

ofxTLVideoDepthAlignmentScrubber::~ofxTLVideoDepthAlignmentScrubber(){

}
	
void ofxTLVideoDepthAlignmentScrubber::setup(){
	enable();
}

void ofxTLVideoDepthAlignmentScrubber::draw(){
	
	if(!ready()){
		ofPushStyle();
		ofSetColor(255, 100, 0, 30);
		ofRect(bounds);
		ofPopStyle();
		return;
	}
		
	ofPushStyle();
	ofSetColor(255, 100, 0);
	for(int i = 0; i < alignedFrames.size(); i++){
		int screenX = screenXForIndex( alignedFrames[i].videoFrame, videoSequence->videoThumbs.size());
		ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
		ofDrawBitmapString("video: " + ofToString(alignedFrames[i].videoFrame), ofPoint(screenX+10, bounds.y+15));
		ofDrawBitmapString("depth: " + ofToString(alignedFrames[i].depthFrame), ofPoint(screenX+10, bounds.y+35));
	}
	
	ofSetColor(0, 125, 255);
	int selectedScreenX = normalizedXtoScreenX(selectedPercent);
	ofLine(selectedScreenX, bounds.y, selectedScreenX, bounds.y+bounds.height);
	ofDrawBitmapString("sel.video: " + ofToString(selectedVideoFrame), ofPoint(selectedScreenX+10, bounds.y+55));
	ofDrawBitmapString("sel.depth: " + ofToString(selectedDepthFrame), ofPoint(selectedScreenX+10, bounds.y+75));
	
	ofPopStyle();
}

void ofxTLVideoDepthAlignmentScrubber::mousePressed(ofMouseEventArgs& args){
}

void ofxTLVideoDepthAlignmentScrubber::mouseMoved(ofMouseEventArgs& args){
}

void ofxTLVideoDepthAlignmentScrubber::mouseDragged(ofMouseEventArgs& args){
	if(ready() && bounds.inside(args.x, args.y)){
		selectedPercent = screenXtoNormalizedX(args.x);
		selectedVideoFrame = indexForScreenX(args.x, videoSequence->videoThumbs.size());
		selectedDepthFrame = getDepthFrameForVideoFrame(selectedVideoFrame);
		videoSequence->selectFrame(selectedVideoFrame);
		depthSequence->selectFrame(selectedDepthFrame);
	}
}

void ofxTLVideoDepthAlignmentScrubber::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLVideoDepthAlignmentScrubber::addAlignedPair(int videoFrame, int depthFrame){
	VideoDepthPair p;
	p.videoFrame = videoFrame;
	p.depthFrame = depthFrame;
	alignedFrames.push_back(p);
	sort(alignedFrames.begin(), alignedFrames.end(), pairsort);

	cout << "addded " << videoFrame << " " << depthFrame << endl;

	save();
}

void ofxTLVideoDepthAlignmentScrubber::removeAlignmentPair(int index){
	alignedFrames.erase(alignedFrames.begin()+index);
	save();
}

void ofxTLVideoDepthAlignmentScrubber::save(){
	if(xmlFileName == ""){
		ofLogError("ofxTLVideoDepthAlignmentScrubber -- saving with no save file");
		return;
	}
	
	ofxXmlSettings settings;
	for(int i = 0; i < alignedFrames.size(); i++){
		settings.addTag("pair");
		settings.pushTag("pair", i);
		settings.addValue("video", alignedFrames[i].videoFrame);
		settings.addValue("depth", alignedFrames[i].depthFrame);
		settings.popTag();
	}

	settings.saveFile(xmlFileName);
}

void ofxTLVideoDepthAlignmentScrubber::load(){
	if(xmlFileName == ""){
		ofLogError("ofxTLVideoDepthAlignmentScrubber -- loading no save file");
		return;
	}
	
	ofxXmlSettings settings;
	if(settings.loadFile(xmlFileName)){
		int numPairs = settings.getNumTags("pair");
		for(int i = 0; i < numPairs; i++){
			settings.pushTag("pair", i);
			VideoDepthPair p;
			p.videoFrame = settings.getValue("video", 0);
			p.depthFrame = settings.getValue("depth", 0);
			alignedFrames.push_back(p);			
			settings.popTag();
			cout << "ofxTLVideoDepthAlignmentScrubber -- Loaded Pairs vid. " << p.videoFrame << " dep. " << p.depthFrame << endl;
		}
	}
	else{
		ofLogError("ofxTLVideoDepthAlignmentScrubber -- error loading file " + xmlFileName);
	}
}

vector<VideoDepthPair> & ofxTLVideoDepthAlignmentScrubber::getPairs(){
	return alignedFrames;
}

int ofxTLVideoDepthAlignmentScrubber::getDepthFrameForVideoFrame(int videoFrame){
	int startIndex, endIndex;
	if(videoFrame < alignedFrames[0].videoFrame){
		startIndex = 0;
		endIndex = 1;
	}
	if(videoFrame > alignedFrames[alignedFrames.size()-1].videoFrame){
		startIndex = alignedFrames.size()-2;
		endIndex = alignedFrames.size()-1;
	}
	else {
		startIndex = 0;
		endIndex = 1;
		while(videoFrame > alignedFrames[endIndex].videoFrame ){
			startIndex++;
			endIndex++;
		}
	}

	if(endIndex == alignedFrames.size()){
		startIndex--;
		endIndex--;
	}
	
	//cout << "looking for index " << videoFrame << " found to be between " << startIndex << " and " << endIndex << endl;
	
	return ofMap(videoFrame, alignedFrames[startIndex].videoFrame, alignedFrames[endIndex].videoFrame,
							 alignedFrames[startIndex].depthFrame, alignedFrames[endIndex].depthFrame, false);
	
}

bool ofxTLVideoDepthAlignmentScrubber::ready(){
	return videoSequence != NULL && depthSequence != NULL && alignedFrames.size() >= 2;
}
