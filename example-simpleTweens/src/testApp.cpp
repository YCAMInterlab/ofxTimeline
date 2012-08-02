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
    timeline.setFrameRate(30);
	timeline.setDurationInFrames(90);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	timeline.addKeyframes("Rotate X", ofRange(0, 360));
	timeline.addKeyframes("Rotate Y", ofRange(0, 360));
	
	timeline.addFlags("Colors");
	
	timeline.getColors().load("defaultColors.xml");
	
     //setting framebased to true results in the timeline never skipping frames
    timeline.setFrameBased(false);

	ofAddListener(timeline.events().bangFired, this, &testApp::receivedBang);

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
	
	ofBox(0,0,0, 200);
	
	ofPopMatrix();

	timeline.draw();
}

//--------------------------------------------------------------
void testApp::receivedBang(ofxTLBangEventArgs& trigger){
    if(trigger.track->getName() == "Colors"){
        if(trigger.flag == "RED"){
            currentColor = ofColor(255,0,0);
        }
        else if(trigger.flag == "GREEN"){
            currentColor = ofColor(0,255,0);		
        }
        else if(trigger.flag == "BLUE"){
            currentColor = ofColor(0,0,255);
        }
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
	