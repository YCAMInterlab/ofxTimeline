#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    ofBackground(255*.15);
    ofSetVerticalSync(true);
	ofEnableSmoothing();

    ofxTimeline::removeCocoaMenusFromGlut("AllTracksExample");
	timeline.setup();

	timeline.addVideoTrack("fingers", "fingers.mov");
  #ifdef TIMELINE_AUDIO_INCLUDED
    timeline.addAudioTrack("audio", "4chan.wav");
    timeline.setDurationInSeconds(timeline.getAudioTrack("audio")->getDuration());
  #endif

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
	
	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
}

//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& args){
	cout << "bang fired!" << args.flag << endl;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	if(timeline.isSwitchOn("switches")){
		ofBackground(timeline.getColor("colors"));
	}
	else{
		ofBackground(255*.15);
	}
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
