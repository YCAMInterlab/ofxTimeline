/*
 *  ofxTLAudioWaveform.cpp
 *  audiowaveformTimelineExample
 *
 *  Created by Jim on 12/28/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLAudioWaveform.h"
#include "ofxTimeline.h"

ofxTLAudioWaveform::ofxTLAudioWaveform(){
	zoomChanging = false;
	shouldRecomputePreview = false;
//	preview.setClosed(false);
}

ofxTLAudioWaveform::~ofxTLAudioWaveform(){

}

void ofxTLAudioWaveform::setup(){
	enable();
	ofxTLRegisterZoomEvents(this);
	ofAddListener(ofxTLEvents.viewWasResized, this, &ofxTLAudioWaveform::boundsChanged);
}

void ofxTLAudioWaveform::loadSoundfile(string filepath){
	player.loadSound(filepath, false);
	cout << "duration is " << player.getDuration() << endl;
	cout << "num samples " << player.getBuffer().size() << endl;
	shouldRecomputePreview = true;
}

float ofxTLAudioWaveform::getDuration(){
	return player.getDuration();
}

void ofxTLAudioWaveform::update(ofEventArgs& args){
	if(player.getPosition() < lastPercent){
		ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(ofxTLEvents.playbackLooped, args);
	}
	lastPercent = player.getPosition();
 
	if(timeline->getIsFrameBased()){
		//TODO: figure out how to do this with a frame based timeline
	}
	else{
		timeline->setCurrentTime(player.getPosition() * player.getDuration());
	}
}

void ofxTLAudioWaveform::draw(){
	
	if(shouldRecomputePreview){
		recomputePreview();
	}
	
	if(player.getBuffer().size() == 0){
		ofSetColor(timeline->getColors().disabledColor);
		ofRectangle(bounds);
		return;
	}
	
	if(!timeline->getIsFrameBased()){
		ofPushStyle();
		ofSetColor(timeline->getColors().keyColor);
		ofNoFill();
		
		for(int i = 0; i < previews.size(); i++){
			ofPushMatrix();
			ofTranslate( normalizedXtoScreenX(computedZoomBounds.min, zoomBounds)  - normalizedXtoScreenX(zoomBounds.min, zoomBounds), 0, 0);
			ofScale(computedZoomBounds.span()/zoomBounds.span(), 1, 1);
			previews[i].draw();
			ofPopMatrix();
		}
		ofPopStyle();
	}
	else {
		//TODO implement frame based with audio waveform
	}	
}

void ofxTLAudioWaveform::recomputePreview(){
	
	previews.clear();
	
//	cout << "recomputing view with zoom bounds of " << zoomBounds << endl;
	
	float normalizationRatio = timeline->getDurationInSeconds() / player.getDuration(); //need to figure this out for framebased...but for now we are doing time based
	float trackHeight = bounds.height/(1+player.getNumChannels());
	int numSamples = player.getBuffer().size() / player.getNumChannels();
	int pixelsPerSample = numSamples / bounds.width;
	for(int c = 0; c < player.getNumChannels(); c++){
		ofPolyline preview;
		int lastFrameIndex = 0;
		for(float i = bounds.x; i < bounds.x+bounds.width; i++){
			float pointInTrack = screenXtoNormalizedX( i, zoomBounds ) * normalizationRatio; //will scale the screenX into wave's 0-1.0
			float trackCenter = bounds.y + trackHeight * (c+1);
			if(pointInTrack <= 1.0){
				//draw sample at pointInTrack * waveDuration;
				int frameIndex = pointInTrack * numSamples;					
				float losample = 0;
				float hisample = 0;
				for(int f = lastFrameIndex; f < frameIndex; f++){
					int sampleIndex = f * player.getNumChannels() + c;
					float subpixelSample = player.getBuffer()[sampleIndex]/32565.0;
					if(subpixelSample < losample) {
						losample = subpixelSample;
					}
					if(subpixelSample > hisample) {
						hisample = subpixelSample;
					}
				}
				if(losample == 0 && hisample == 0){
					preview.addVertex(i, trackCenter);
				}
				else {
					if(losample != 0){
						preview.addVertex(i, trackCenter - losample * trackHeight);
					}
					if(hisample != 0){
						//ofVertex(i, trackCenter - hisample * trackHeight);
						preview.addVertex(i, trackCenter - hisample * trackHeight);
					}
				}
				lastFrameIndex = frameIndex;
			}
			else{
				preview.addVertex(i,trackCenter);
			}
		}
		previews.push_back(preview);
	}
	computedZoomBounds = zoomBounds;
	shouldRecomputePreview = false;
}

void ofxTLAudioWaveform::mousePressed(ofMouseEventArgs& args){
}

void ofxTLAudioWaveform::mouseMoved(ofMouseEventArgs& args){
}

void ofxTLAudioWaveform::mouseDragged(ofMouseEventArgs& args, bool snapped){
}

void ofxTLAudioWaveform::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLAudioWaveform::keyPressed(ofKeyEventArgs& args){
}

void ofxTLAudioWaveform::zoomStarted(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomStarted(args);
//	shouldRecomputePreview = true;
}

void ofxTLAudioWaveform::zoomDragged(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomDragged(args);
	//shouldRecomputePreview = true;
}

void ofxTLAudioWaveform::zoomEnded(ofxTLZoomEventArgs& args){
	ofxTLElement::zoomEnded(args);
	shouldRecomputePreview = true;
}

void ofxTLAudioWaveform::boundsChanged(ofEventArgs& args){
	shouldRecomputePreview = true;
}

void ofxTLAudioWaveform::play(){
	if(!player.getIsPlaying()){
		
		lastPercent = MIN(timeline->getPercentComplete() * timeline->getDurationInSeconds() / player.getDuration(), 1.0);
		player.setPosition(lastPercent);
		player.setLoop(timeline->getLoopType() == OF_LOOP_NORMAL);

		player.play();
		ofAddListener(ofEvents.update, this, &ofxTLAudioWaveform::update);
		
		ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(ofxTLEvents.playbackStarted, args);		
	}	   
}

void ofxTLAudioWaveform::stop(){
	if(player.getIsPlaying()){
		player.setPaused(true);
		ofRemoveListener(ofEvents.update, this, &ofxTLAudioWaveform::update);
		
		ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(ofxTLEvents.playbackEnded, args);
	}
}

void ofxTLAudioWaveform::togglePlay(){
	if(player.getIsPlaying()){
		stop();
	}
	else {
		play();
	}
}
   
