#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
    
	glEnable(GL_DEPTH_TEST);
	ofEnableLighting();
	
	currentColor = ofColor(255);
	
	light.setPosition(ofGetWidth()*.5, ofGetHeight()*.25, 0);
	light.enable();
		
    //Timeline setup and playback details
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInFrames(90);
	timeline.setLoopType(OF_LOOP_NORMAL);
    
	//each call to "add keyframes" add's another track to the timeline
	timeline.addCurves("Rotate X", ofRange(0, 360));
	timeline.addCurves("Rotate Y", ofRange(0, 360));
    
	//Flags are little markers that you can attach text to
    //They are only useful when listening to bangFired() events
    //so that you know when one has passed
	timeline.addFlags("Colors");
	
	timeline.getColors().load("defaultColors.xml");
	
    //setting framebased to true results in the timeline never skipping frames
    //and also speeding up with the 
    //try setting this to true and see the difference
    timeline.setFrameBased(false);
	
    //Bang events are fired whenever a Bang or a Flag is passed in the timeline
	ofAddListener(timeline.events().bangFired, this, &testApp::receivedBang);

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(.15*255);
	ofPushMatrix();
	
    //set the color to whatever the last color we encountered was
	ofSetColor(currentColor);
	
    //translate to the center of the screen
	ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5, 40);	
    
    //Read the values out of the timeline and use them to change the viewport rotation
	ofRotate(timeline.getValue("Rotate X"), 1, 0, 0);
	ofRotate(timeline.getValue("Rotate Y"), 0, 1, 0);
	
	ofBox(0,0,0, 200);
	
	ofPopMatrix();

	timeline.draw();
}

//--------------------------------------------------------------
void testApp::receivedBang(ofxTLBangEventArgs& trigger){
    
    //Preset some colors, these are typed in by hand
    //on the flags through the editor
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
	