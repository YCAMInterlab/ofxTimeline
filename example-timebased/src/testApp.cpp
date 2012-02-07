#include "testApp.h"
#include "ofxTLUtils.h"

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
	timeline.setDurationInSeconds(60);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	timeline.addKeyframes("Rotate X", "rotatex.xml", ofRange(0, 360));
	timeline.addKeyframes("Rotate Y", "rotatey.xml", ofRange(0, 360));
	
	timeline.addTriggers("Colors", "colors.xml");
	timeline.addSwitcher("big box", "switcher.xml");
	
	timeline.getColors().loadColors("defaultColors.xml");

	timeline.setSnapping(true);
	timeline.enableSnapToBPM(120.f);
	timeline.enableDrawBPMGrid(true);
	timeline.enableSnapToOtherElements(true);
	timeline.setMovePlayheadOnDrag(false);
	timeline.setMovePlayheadOnPaste(true);
	
	ofAddListener(ofxTLEvents.trigger, this, &testApp::receivedTrigger);	
	
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
	ofRotate(timeline.getKeyframeValue("Rotate X"), 1, 0, 0);
	ofRotate(timeline.getKeyframeValue("Rotate Y"), 0, 1, 0);
	
	if (timeline.getSwitcherOn("big box")) {
		ofBox(0,0,0,200);
	}
	else {
		ofBox(0,0,0,50);
	}
	
	ofPopMatrix();

	timeline.draw();
	
}

//--------------------------------------------------------------
void testApp::receivedTrigger(ofxTLTriggerEventArgs& trigger){
	if(trigger.triggerName == "RED"){
		currentColor = ofColor(255,0,0);
	}
	else if(trigger.triggerName == "GREEN"){
		currentColor = ofColor(0,255,0);		
	}
	else if(trigger.triggerName == "BLUE"){
		currentColor = ofColor(0,0,255);
	}
	else if(trigger.triggerName == "MAROON"){
		currentColor = ofColor(255,0,198);
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' '){
		timeline.togglePlay();
	}
	if(key == 'h'){
		timeline.toggleShow();
	}
	if(key == 'H'){
		timeline.collapseAllElements();
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
