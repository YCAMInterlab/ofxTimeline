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

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(0);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	

    ofxTimeline::removeCocoaMenusFromGlut("Audio Waveform Example");
    
	timeline.setup();
	timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setBPM(120.f);
	timeline.enableSnapToBPM(true);
	timeline.setShowBPMGrid(true);
	timeline.addTrack("Track", &waveform);
	
	//this means that calls to play/stop etc will be  routed to the waveform and that timing will be 100% accurate
	timeline.setTimecontrolTrack(&waveform);

	waveform.loadSoundfile("4chan.wav");


	//fun to watch on FFT
	//waveform.loadSoundfile("audiocheck.net_sweep20-20klog.wav");
	//waveform.loadSoundfile("audiocheck.net_sweep20-20klin.wav");
	
	timeline.setDurationInSeconds(waveform.getDuration());

}

//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
    
    //change the background color based on the current bin and amount
    if(waveform.isSoundLoaded()){
        int bin = ofMap(mouseX, 0, ofGetWidth(), 0, waveform.getFFTBinCount()-1, true);
        ofBackground( waveform.getFFTSpectrum()[bin] * 255 );
    }
    
	timeline.draw();
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
