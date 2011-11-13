#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	timeline = new ofxTimeline();
	timeline->setup();
	timeline->addImageSequence("testsequence");
	timeline->addKeyframes("Keyframer A", "keyframe_a.xml", ofRange(0, 1.0));
	
	/*
	timeline->addKeyframes("Keyframe B", "keyframe_b.xml", ofRange(0, 1.0));
	timeline->addKeyframes("Keyframe C", "keyframe_c.xml", ofRange(0, 1.0));

	timeline->addPage("second page", true);
	timeline->addKeyframes("Keyframe 1", "keyframe_1.xml", ofRange(0, 1.0));
	timeline->addKeyframes("Keyframe 2", "keyframe_2.xml", ofRange(0, 1.0));
	*/
	
	timeline->setDuration(300);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(.15*255);
	
	timeline->draw();
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
	timeline->setWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
