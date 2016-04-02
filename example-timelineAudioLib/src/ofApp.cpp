/**
 * ofxTimeline -- AudioWaveform Example
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George http://www.jamesgeorge.org
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/ +
 *
 * http://github.com/YCAMinterLab
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
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
    ofBackground(0);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
	

    ofxTimeline::removeCocoaMenusFromGlut("Audio Waveform Example");
    
    timeline.setup();
    timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setBPM(120.f);
    timeline.enableSnapToBPM(true);
    timeline.setShowBPMGrid(true);
    timeline.addAudioTrack("Audio","4chan.wav");
    
    //this means that calls to play/stop etc will be  routed to the waveform and that timing will be 100% accurate
    timeline.setTimecontrolTrack("Audio");

    //fun to watch on FFT
    //waveform.loadSoundfile("audiocheck.net_sweep20-20klog.wav");
    //waveform.loadSoundfile("audiocheck.net_sweep20-20klin.wav");

    timeline.setDurationInSeconds(timeline.getAudioTrack("Audio")->getDuration());

}

//--------------------------------------------------------------
void ofApp::update(){
	
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //change the background color based on the current bin and amount

    ofxTLAudioTrack* track = timeline.getAudioTrack("Audio");
    int bin = ofMap(mouseX, 0, ofGetWidth(), 0, track->getFFTSize()-1, true);
    ofBackground( track->getFFT()[bin] * 2000 );
    
    timeline.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
