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

}

ofxTLAudioWaveform::~ofxTLAudioWaveform(){

}

void ofxTLAudioWaveform::setup(){
	enable();
}

void ofxTLAudioWaveform::loadSoundfile(string filepath){
	player.loadSound(filepath, false);
	cout << "duration is " << player.getDuration() << endl;
	cout << "num samples " << player.getBuffer().size() << endl;
}

void ofxTLAudioWaveform::draw(){
	if(player.getBuffer().size() == 0){
		ofSetColor(timeline->getColors().disabledColor);
		ofRectangle(bounds);
		return;
	}
	if(!timeline->getIsFrameBased()){
		ofPushStyle();
		ofSetColor(timeline->getColors().keyColor);
		
		ofNoFill();
		
		float normalizationRatio = timeline->getDurationInSeconds() / player.getDuration(); //need to figure this out for framebased...but for now we are doing time based
		float trackHeight = bounds.height/(1+player.getNumChannels());
		for(int c = 0; c < player.getNumChannels(); c++){
			int lastFrameIndex = 0;
			ofBeginShape();
			for(float i = bounds.x; i < bounds.x+bounds.width; i++){
				float pointInTrack = screenXtoNormalizedX( i, zoomBounds ) * normalizationRatio; //will scale the screenX into wave's 0-1.0
				float trackCenter = bounds.y + trackHeight * (c+1);
				if(pointInTrack <= 1.0){
					//draw sample at pointInTrack * waveDuration;
					int frameIndex = pointInTrack * (player.getBuffer().size() / player.getNumChannels());					
					float sample = 0;
					for(int f = lastFrameIndex; f < frameIndex; f++){		
						int sampleIndex = f * player.getNumChannels() + c;
						float subpixelSample = player.getBuffer()[sampleIndex]/32565.0;
						if ( fabs(subpixelSample) > fabs(sample)) {
							sample = subpixelSample;
						}

					}
					ofVertex(i, trackCenter - sample * trackHeight);
					lastFrameIndex = frameIndex;
				}
				else{
					ofVertex(i, trackCenter);
				}
			}
			ofEndShape();
		}
		
		ofPopStyle();
	}
	else {
		//TODO implement frame based with audio waveform
	}
	
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
