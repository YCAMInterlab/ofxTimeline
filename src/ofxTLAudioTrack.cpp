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
    useEnvelope = true;
    dampening = .1;
	lastFFTPosition = -1;
	defaultSpectrumBandwidth = 1024;
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
        player.getSpectrum(defaultSpectrumBandwidth);
        setFFTLogAverages();
        averageSize = player.getAverages().size();
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

				player.setPosition( positionForSecond(timeline->getInTimeInSeconds())+.001 );
			}
			else if(lastPercent > timeline->getInOutRange().max){
				if(timeline->getLoopType() == OF_LOOP_NONE){
					player.setPosition( positionForSecond(timeline->getInTimeInSeconds()));
					stop();
				}
				else{
					player.setPosition( positionForSecond(timeline->getInTimeInSeconds()));
				}
			}
			
			timeline->setCurrentTimeSeconds(player.getPosition() * player.getDuration());
		}
	}
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
        ofTranslate( normalizedXtoScreenX(computedZoomBounds.min, zoomBounds) - normalizedXtoScreenX(zoomBounds.min, zoomBounds), 0, 0);
        ofScale(computedZoomBounds.span()/zoomBounds.span(), 1, 1);
        previews[i].draw();
        ofPopMatrix();
    }
    ofPopStyle();
	

    
    ofPushStyle();
    
    //will refresh fft bins for other calls too
    vector<float>& bins = getFFT();
    float binWidth = bounds.width / bins.size();
    
    ofFill();
    ofSetColor(timeline->getColors().disabledColor, 120);
    for(int i = 0; i < bins.size(); i++){
        float height = MIN(bounds.height * bins[i], bounds.height);
        float y = bounds.y + bounds.height - height;
        ofRect(i*binWidth, y, binWidth, height);
    }
    
    ofPopStyle();
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
		preview.resize(bounds.width*2);  //Why * 2? Because there are two points per pixel, center and outside. 
		for(float i = bounds.x; i < bounds.x+bounds.width; i++){
			float pointInTrack = screenXtoNormalizedX( i ) * normalizationRatio; //will scale the screenX into wave's 0-1.0
			float trackCenter = bounds.y + trackHeight * (c+1);
			
			ofPoint * vertex = & preview.getVertices()[ (i - bounds.x) * 2];
			
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
						vertex->y = trackCenter - losample * trackHeight*.5;
						vertex++;
					}
					if(hisample != 0){
						//ofVertex(i, trackCenter - hisample * trackHeight);
//						preview.addVertex(i, trackCenter - hisample * trackHeight);
						vertex->x = i;
						vertex->y = trackCenter - hisample * trackHeight*.5;
						vertex++;
					}
				}
				
				while (vertex < & preview.getVertices()[ (i - bounds.x) * 2] + 2) {
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
			if(player.getPosition() == 1.0 || timeline->getPercentComplete() > .99){
                timeline->setCurrentTimeSeconds(0);
            }
            
            player.setPosition(positionForSecond(timeline->getCurrentTime()));
            //cout << " setting time to  " << positionForSecond(timeline->getCurrentTime()) << " actual " << player.getPosition() << endl;
            
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

void ofxTLAudioTrack::setUseFFTEnvelope(bool useFFTEnveolope){
    useEnvelope = useFFTEnveolope;
}

bool ofxTLAudioTrack::getUseFFTEnvelope(){
    return useEnvelope;
}

void ofxTLAudioTrack::setFFTDampening(float damp){
    dampening = damp;
}

float ofxTLAudioTrack::getFFTDampening(){
    return dampening;
}

void ofxTLAudioTrack::setFFTLogAverages(int minBandwidth, int bandsPerOctave){
    if(isSoundLoaded()){
        player.setLogAverages(minBandwidth, bandsPerOctave);
    }
}

int ofxTLAudioTrack::getLogAverageMinBandwidth(){
    return player.getMinBandwidth();
}

int ofxTLAudioTrack::getLogAverageBandsPerOctave(){
    return player.getBandsPerOctave();
}

int ofxTLAudioTrack::getFFTSize(){
    return averageSize;
}

//envelope and dampening approach from Marius Watz
//http://workshop.evolutionzone.com/2012/08/30/workshops-sept-89-sound-responsive-visuals-3d-printing-and-parametric-modeling/
vector<float>& ofxTLAudioTrack::getFFT(){
	float fftPosition = player.getPosition();
	if(isSoundLoaded() && lastFFTPosition != fftPosition){

        vector<float>& fftAverages = player.getAverages();
        averageSize = fftAverages.size();
        if(envelope.size() != averageSize){
            generateEnvelope(averageSize);
        }
        
        if(dampened.size() != averageSize){
            dampened.clear();
            dampened.resize(averageSize);
        }

        if(getUseFFTEnvelope()){
            for(int i = 0; i < fftAverages.size(); i++){
                fftAverages[i] *= envelope[i];
            }
        }
        
        float max = 0;
        for(int i = 0; i < fftAverages.size(); i++){
            max = MAX(max, fftAverages[i]);
        }
        if(max != 0){
            for(int i = 0; i < fftAverages.size(); i++){
                fftAverages[i] = ofMap(fftAverages[i],0, max, 0, 1.0);
            }
        }
 
        for(int i = 0; i < averageSize; i++) {
            dampened[i] = (fftAverages[i] * dampening) + dampened[i]*(1-dampening);
        }
        
        //normalizer hack
        lastFFTPosition = fftPosition;
	}
    
	return dampened;
}

void ofxTLAudioTrack::generateEnvelope(int size){
    envelope.clear();
    
    for(int i = 0; i < size; i++) {
        envelope.push_back(ofBezierPoint(ofPoint(0.05,0),
                                         ofPoint(0.1, 0),
                                         ofPoint(0.2, 0),
                                         ofPoint(1.0, 0),
                                         ofMap(i, 0,size-1, 0,1) ).x);
    }
}

string ofxTLAudioTrack::getTrackType(){
    return "Audio";    
}

   
