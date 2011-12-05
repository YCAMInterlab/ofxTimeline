#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
	
	currentColor = ofColor(255);
	
	light.setPosition(ofGetWidth()*.5, ofGetHeight()*.25, 0);
	light.enable();
	
	timeline.setup();
	timeline.setDurationInFrames(90);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	timeline.addKeyframes("RotateX", "rotate_x.xml", ofRange(0, 360));
	timeline.addKeyframes("RotateY", "rotate_y.xml", ofRange(0, 360));
	
	timeline.addTriggers("Color", "color_triggers.xml");
	ofAddListener(ofxTLEvents.trigger, this, &testApp::timelineTriggerReceived);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(.15*255);
	ofPushMatrix();
	
	ofSetColor(currentColor);
	
	ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5, 40);	
	ofRotate(timeline.getKeyframeValue("RotateX"), 1, 0, 0);
	ofRotate(timeline.getKeyframeValue("RotateY"), 0, 1, 0);
	
	ofBox(0,0,0, 200);
	
	ofPopMatrix();
	
	timeline.draw();
}

//--------------------------------------------------------------
void testApp::timelineTriggerReceived(ofxTLTriggerEventArgs& trigger){
	if(trigger.triggerName == "RED"){
		currentColor = ofColor(255,0,0);
	}
	else if(trigger.triggerName == "GREEN"){
		currentColor = ofColor(0,255,0);		
	}
	else if(trigger.triggerName == "BLUE"){
		currentColor = ofColor(0,0,255);
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		timeline.togglePlay();
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
