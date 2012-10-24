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

#include "ofxTLAudioTrack.h"
#include "ofxTimeline.h"

ofxTLAudioTrack::ofxTLAudioTrack(){
	shouldRecomputePreview = false;
    soundLoaded = false;
	lastFFTPosition = 0;
	defaultFFTBins = 256;
	maxBinReceived = 0;
}

ofxTLAudioTrack::~ofxTLAudioTrack(){

}

bool ofxTLAudioTrack::loadSoundfile(string filepath){
	soundLoaded = false;
	if(player.loadSound(filepath, false)){
    	soundLoaded = true;
		soundFilePath = filepath;
		shouldRecomputePreview = true;
    }
	return soundLoaded;
}
 
string ofxTLAudioTrack::getSoundfilePath(){
	return soundFilePath;
}

bool ofxTLAudioTrack::isSoundLoaded(){
	return soundLoaded;
}

float ofxTLAudioTrack::getDuration(){
	return player.getDuration();
}

void ofxTLAudioTrack::update(){
	if(this == timeline->getTimecontrolTrack()){
		if(getIsPlaying()){
			if(player.getPosition() < lastPercent){
				ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
				ofNotifyEvent(events().playbackLooped, args);
			}
			lastPercent = player.getPosition();
			
			//currently only supports timelines with duration == duration of player
			if(lastPercent < timeline->getInOutRange().min){
				//player.setPosition( timeline->getInOutRange().min );
				player.setPosition( positionForSecond(timeline->getInTimeInSeconds()) );
			}
			else if(lastPercent > timeline->getInOutRange().max){
				if(timeline->getLoopType() == OF_LOOP_NONE){
//					player.setPosition(timeline->getInOutRange().max);
					player.setPosition( positionForSecond(timeline->getInTimeInSeconds()) );
					stop();
				}
				else{
//					player.setPosition(timeline->getInOutRange().min);
					player.setPosition( positionForSecond(timeline->getOutTimeInSeconds()) );
				}
			}
			
			timeline->setCurrentTimeSeconds(player.getPosition() * player.getDuration());
		}
	}
	//cout << "audio track " << player.getPosition() << " timeline " << timeline->getPercentComplete() << endl;
}
 
void ofxTLAudioTrack::draw(){
	
	if(!soundLoaded || player.getBuffer().size() == 0){
		ofPushStyle();
		ofSetColor(timeline->getColors().disabledColor);
		ofRectangle(bounds);
		ofPopStyle();
		return;
	}
		
	if(shouldRecomputePreview || viewIsDirty){
//		cout << "recomputing waveform for audio file " << getSoundfilePath() << endl;
		recomputePreview();
	}


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
	
	if(getIsPlaying() || timeline->getIsPlaying()){
		ofPushStyle();
		
		//will refresh fft bins for other calls too
		vector<float>& bins = getFFTSpectrum(defaultFFTBins);
		float binWidth = bounds.width / bins.size();
		//find max
		float averagebin = 0 ;
		for(int i = 0; i < bins.size(); i++){
			maxBinReceived = MAX(maxBinReceived, bins[i]);
			averagebin += bins[i];
		}
		averagebin /= bins.size();
		
		ofFill();
		ofSetColor(timeline->getColors().disabledColor, 120);
		for(int i = 0; i < bins.size(); i++){
			float height = bounds.height * bins[i]/maxBinReceived;
			float y = bounds.y + bounds.height - height;
			ofRect(i*binWidth, y, binWidth, height);
		}
		
		ofPopStyle();
	}
}

float ofxTLAudioTrack::positionForSecond(float second){
	if(isSoundLoaded()){
		return ofMap(second, 0, player.getDuration(), 0, 1.0, true);
	}
	return 0;
}

void ofxTLAudioTrack::recomputePreview(){
	
	previews.clear();
	
//	cout << "recomputing view with zoom bounds of " << zoomBounds << endl;
	
	float normalizationRatio = timeline->getDurationInSeconds() / player.getDuration(); //need to figure this out for framebased...but for now we are doing time based
	float trackHeight = bounds.height/(1+player.getNumChannels());
	int numSamples = player.getBuffer().size() / player.getNumChannels();
	int pixelsPerSample = numSamples / bounds.width;
	int numChannels = player.getNumChannels();
	vector<short> & buffer  = player.getBuffer();

	for(int c = 0; c < numChannels; c++){
		ofPolyline preview;
		int lastFrameIndex = 0;
		preview.getVertices().resize(bounds.width*2);
		for(float i = bounds.x; i < bounds.x+bounds.width; i++){
			float pointInTrack = screenXtoNormalizedX( i ) * normalizationRatio; //will scale the screenX into wave's 0-1.0
			float trackCenter = bounds.y + trackHeight * (c+1);
			
			ofPoint * vertex = & preview.getVertices()[i*2];
			
			if(pointInTrack >= 0 && pointInTrack <= 1.0){
				//draw sample at pointInTrack * waveDuration;
				int frameIndex = pointInTrack * numSamples;					
				float losample = 0;
				float hisample = 0;
				for(int f = lastFrameIndex; f < frameIndex; f++){
					int sampleIndex = f * numChannels + c;
					float subpixelSample = buffer[sampleIndex]/32565.0;
					if(subpixelSample < losample) {
						losample = subpixelSample;
					}
					if(subpixelSample > hisample) {
						hisample = subpixelSample;
					}
				}
				
				if(losample == 0 && hisample == 0){
					//preview.addVertex(i, trackCenter);
					vertex->x = i;
					vertex->y = trackCenter;
					vertex++;
				}
				else {
					if(losample != 0){
//						preview.addVertex(i, trackCenter - losample * trackHeight);
						vertex->x = i;
						vertex->y = trackCenter - losample * trackHeight;
						vertex++;
					}
					if(hisample != 0){
						//ofVertex(i, trackCenter - hisample * trackHeight);
//						preview.addVertex(i, trackCenter - hisample * trackHeight);
						vertex->x = i;
						vertex->y = trackCenter - hisample * trackHeight;
						vertex++;
					}
				}
				
				while (vertex < & preview.getVertices()[i*2] + 2) {
					*vertex = *(vertex-1);
					vertex++;
				}

				lastFrameIndex = frameIndex;
			}
			else{
				*vertex++ = ofPoint(i,trackCenter);
				*vertex++ = ofPoint(i,trackCenter);
			}
		}
		preview.simplify();
		previews.push_back(preview);
	}
	computedZoomBounds = zoomBounds;
	shouldRecomputePreview = false;
}

int ofxTLAudioTrack::getDefaultBinCount(){
//	cout << defaultFFTBins << endl;
	return defaultFFTBins;
}

bool ofxTLAudioTrack::mousePressed(ofMouseEventArgs& args, long millis){
	return false;
}

void ofxTLAudioTrack::mouseMoved(ofMouseEventArgs& args, long millis){
}

void ofxTLAudioTrack::mouseDragged(ofMouseEventArgs& args, long millis){
}

void ofxTLAudioTrack::mouseReleased(ofMouseEventArgs& args, long millis){
}

void ofxTLAudioTrack::keyPressed(ofKeyEventArgs& args){
}

void ofxTLAudioTrack::zoomStarted(ofxTLZoomEventArgs& args){
	ofxTLTrack::zoomStarted(args);
//	shouldRecomputePreview = true;
}

void ofxTLAudioTrack::zoomDragged(ofxTLZoomEventArgs& args){
	ofxTLTrack::zoomDragged(args);
	//shouldRecomputePreview = true;
}

void ofxTLAudioTrack::zoomEnded(ofxTLZoomEventArgs& args){
	ofxTLTrack::zoomEnded(args);
	shouldRecomputePreview = true;
}

void ofxTLAudioTrack::boundsChanged(ofEventArgs& args){
	shouldRecomputePreview = true;
}

void ofxTLAudioTrack::play(){
	if(!player.getIsPlaying()){
		
//		lastPercent = MIN(timeline->getPercentComplete() * timeline->getDurationInSeconds() / player.getDuration(), 1.0);
		player.setLoop(timeline->getLoopType() == OF_LOOP_NORMAL);

		player.play();
		if(timeline->getTimecontrolTrack() == this){
			ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
			ofNotifyEvent(events().playbackStarted, args);
		}
	}
}

void ofxTLAudioTrack::stop(){
	if(player.getIsPlaying()){
		
		player.setPaused(true);

		if(timeline->getTimecontrolTrack() == this){
			ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
			ofNotifyEvent(events().playbackEnded, args);
		}
	}
}

void ofxTLAudioTrack::playbackStarted(ofxTLPlaybackEventArgs& args){
	ofxTLTrack::playbackStarted(args);
	if(isSoundLoaded() && this != timeline->getTimecontrolTrack()){
		//player.setPosition(timeline->getPercentComplete());
		float position = positionForSecond(timeline->getCurrentTime());
		if(position < 1.0){
			player.play();
		}
		player.setPosition( position );
	}
}

void ofxTLAudioTrack::playbackLooped(ofxTLPlaybackEventArgs& args){
	if(isSoundLoaded() && this != timeline->getTimecontrolTrack()){
		if(!player.getIsPlaying()){
			player.play();
		}
		player.setPosition( positionForSecond(timeline->getCurrentTime()) );
	}
}

void ofxTLAudioTrack::playbackEnded(ofxTLPlaybackEventArgs& args){
	if(isSoundLoaded() && this != timeline->getTimecontrolTrack()){
		player.stop();
	}
}

bool ofxTLAudioTrack::togglePlay(){
	if(getIsPlaying()){
		stop();
	}
	else {
		play();
	}
	return getIsPlaying();
}

bool ofxTLAudioTrack::getIsPlaying(){
    return player.getIsPlaying();
}

void ofxTLAudioTrack::setSpeed(float speed){
    player.setSpeed(speed);
}

float ofxTLAudioTrack::getSpeed(){
    return player.getSpeed();
}

void ofxTLAudioTrack::setVolume(float volume){
    player.setVolume(volume);
}

void ofxTLAudioTrack::setPan(float pan){
    player.setPan(pan);
}

vector<float>& ofxTLAudioTrack::getFFTSpectrum(int numBins){
	float fftPosition = player.getPosition();
	if(isSoundLoaded() && lastFFTPosition != fftPosition){
		if(defaultFFTBins != numBins){
			maxBinReceived = 0;
			defaultFFTBins = numBins;
		}
		lastFFTPosition = fftPosition;
		fftBins = player.getSpectrum(defaultFFTBins);
	}
	return fftBins;
}

string ofxTLAudioTrack::getTrackType(){
    return "Audio";    
}

   
