#include "testApp.h"
#include "ofxTLUtils.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	currentColor = ofColor(255);
	
	light.setPosition(ofGetWidth()*.5, ofGetHeight()*.25, 0);
	light.enable();
			
	timeline.setup();
	timeline.setDurationInSeconds(waveform.getDuration());
	timeline.setLoopType(OF_LOOP_NORMAL);
		
	timeline.setSnapping(true);
	timeline.enableSnapToBPM(120.f);
	timeline.enableDrawBPMGrid(true);
	
	waveform.setup();
	waveform.loadSoundfile("4chan.wav");
	timeline.addElement("Track", &waveform);	
}

//--------------------------------------------------------------
void testApp::update(){
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(.15*255);

	timeline.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		waveform.togglePlay();
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
