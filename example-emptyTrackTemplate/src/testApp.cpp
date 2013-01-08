/**
 * Empty Template
 * ofxTimeline
 *
 * Examples of two template classes that can be copied and extended
 * to invent new types of timeline tracks
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(255*.15);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	ofEnableSmoothing();
	ofEnableAlphaBlending();

	//lets you use COMMAND+C and COMMAND+V actions on mac
	ofxTimeline::removeCocoaMenusFromGlut("Empty Templates");
	
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
void testApp::update(){
		
}

//--------------------------------------------------------------
void testApp::draw(){
    float curtime = timeline.getCurrentTime();
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
