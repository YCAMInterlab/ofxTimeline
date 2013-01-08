/**
 * LFO example
 * ofxTimeline
 *
 * simple example for low frequency oscillator track
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(255*.15);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	ofEnableSmoothing();
	ofEnableAlphaBlending();

	//on mac lets you use COMMAND+C and COMMAND+V actions
	ofxTimeline::removeCocoaMenusFromGlut("Empty Templates");
	
	timeline.setup();
	timeline.setLoopType(OF_LOOP_NORMAL);
	timeline.setDurationInSeconds(30);
    
	//this is the simplest example and is really flexible
	timeline.addLFO("LFO");
	
	
}

//--------------------------------------------------------------
void testApp::update(){
		
}

//--------------------------------------------------------------
void testApp::draw(){

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
