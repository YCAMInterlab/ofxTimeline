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
	

	ofSetFrameRate(30);
	ofSetVerticalSync(true);

	ofEnableSmoothing();
	ofEnableAlphaBlending();

	//lets you use COMMAND+C and COMMAND+V actions on mac
	ofxTimeline::removeCocoaMenusFromGlut("Camera Track");
	
	timeline.setup();
	timeline.setLoopType(OF_LOOP_NORMAL);
	timeline.setDurationInSeconds(30);

    cameraTrack = new ofxTLCameraTrack();
	cameraTrack->setCamera(cam);
	timeline.addTrack("Camera", cameraTrack);
	
	cameraTrack->lockCameraToTrack = true;
	timeline.play();
	
	for(int y = -1000; y < 1000; y += 2){
		boxes.push_back(ofVec3f(cos(y/10.0)*abs(y), y, sin(y/10.0)*abs(y)));
	}
			
	
	for(int i = 0; i < 10000; i++){
		particles.addVertex(ofVec3f(ofRandom(-2000,2000),
									ofRandom(-2000,2000),
									ofRandom(-2000,2000)));
							
		particles.addColor(ofFloatColor(ofRandomuf()*.4));
	}

}

//--------------------------------------------------------------
void ofApp::update(){
		
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackgroundGradient(ofColor(255*.15), ofColor::black, OF_GRADIENT_CIRCULAR);

	ofEnableSmoothing();
	glEnable(GL_POINT_SMOOTH);
	glPointSize(.5);
	
	cam.begin();
	for(int i = 0; i < boxes.size(); i++){
		ofSetColor( ofColor::yellowGreen.getLerped(ofColor::blueViolet, 1.0*i/boxes.size()) );
		ofPushMatrix();
		ofRotate(ofGetFrameNum()*i/100.0, 0, 1, 0);
		ofBox(boxes[i], 5);
		ofPopMatrix();
	}
	particles.drawVertices();
	cam.end();
	
	timeline.draw();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if(key == 'L'){
		cameraTrack->lockCameraToTrack = !cameraTrack->lockCameraToTrack;
	}
	else if(key == 'T'){
		cameraTrack->addKeyframe();
	}
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
