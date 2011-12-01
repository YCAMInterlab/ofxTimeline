#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	timeline.setup();
	
	timeline.addKeyframes("Keyframer A", "keyframe_a.xml", ofRange(5, 100));
	timeline.addKeyframes("Keyframer B", "keyframe_b.xml", ofRange(0, 1.0));
	timeline.addKeyframes("Keyframer C", "keyframe_c.xml", ofRange(0, 1.0));

	timeline.addPage("second page", true);
	timeline.addKeyframes("Keyframer 1", "keyframe_1.xml", ofRange(0, 1.0));
	timeline.addKeyframes("Keyframer 2", "keyframe_2.xml", ofRange(0, 1.0));
	
	timeline.setLoopType(OF_LOOP_NORMAL);
	timeline.setDurationInFrames(90);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(.15*255);
	
	timeline.draw();
	float size = timeline.getKeyframeValue("Keyframer A");
	ofRect(ofGetWidth()/2, ofGetHeight()/2, size, size);
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
