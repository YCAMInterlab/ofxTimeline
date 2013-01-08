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
	if(key == ' '){
		timeline.togglePlay();
	}
    
	if(key == 'h'){
		timeline.toggleShow();
	}
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
	timeline.setWidth(w);
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
