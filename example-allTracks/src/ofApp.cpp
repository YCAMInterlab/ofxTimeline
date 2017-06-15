#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
    ofBackground(255*.15);
    ofSetVerticalSync(true);
	ofEnableSmoothing();

    ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
	timeline.setup();

	timeline.addVideoTrack("fingers", "fingers.mov");
    timeline.addAudioTrack("audio", "4chan.wav");
    timeline.setDurationInSeconds(timeline.getVideoTrack("fingers")->getDuration());

	timeline.addCurves("curves", ofRange(0, 255));
	timeline.addBangs("bangs");
	timeline.addFlags("flags");
	timeline.addColors("colors");
	timeline.addLFO("lfo");
	timeline.addSwitches("switches");

	timeline.setPageName("Page 1");
	timeline.addPage("Page 2");
	timeline.addPage("Page 3");
	timeline.addPage("Page 4");
	timeline.setCurrentPage(0);

	timeline.enableSnapToOtherKeyframes(false);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	ofAddListener(timeline.events().bangFired, this, &ofApp::bangFired);
}

//--------------------------------------------------------------
void ofApp::bangFired(ofxTLBangEventArgs& args){
	cout << "bang fired!" << args.flag << endl;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	if(timeline.isSwitchOn("switches")){
		ofBackground(timeline.getColor("colors"));
	}
	else{
		ofBackground(255*.15);
	}
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
