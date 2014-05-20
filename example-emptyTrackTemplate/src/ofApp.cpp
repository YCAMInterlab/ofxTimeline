/**
 * Empty Template
 * ofxTimeline
 *
 * Examples of two template classes that can be copied and extended
 * to invent new types of timeline tracks
 */

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofBackground(255*.15);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	
	timeline.setup();
	timeline.setLoopType(OF_LOOP_NORMAL);
	timeline.setDurationInSeconds(30);
    
	//this is the simplest example and is really flexible
	emptyTrack = new ofxTLEmptyTrack();
	timeline.addTrack("My Custom Track", emptyTrack);

	//a very simple color keyframe
	emptyKeyframes = new ofxTLEmptyKeyframes();
	emptyKeyframes->setXMLFileName("MyEmptyKeyframes.xml");
	timeline.addTrack("My Custom Keyframes", emptyKeyframes);

}

//--------------------------------------------------------------
void ofApp::update(){
		
}

//--------------------------------------------------------------
void ofApp::draw(){
    float curtime = timeline.getCurrentTime();
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
