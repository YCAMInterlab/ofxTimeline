#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofSetEscapeQuitsApp(false);
	ofEnableSmoothing();
	ofEnableAlphaBlending();
	
	timeline.setup();
	timeline.setLoopType(OF_LOOP_NORMAL);
    timeline.setBPM(120.f);
	timeline.enableSnapToBPM(true);
	timeline.setShowBPMGrid(true);
	
	timeline.addTrack("Track", &waveform);
	
	//this means that calls to play/stop etc will be  routed to the waveform
	timeline.setTimecontrolTrack(&waveform);
    waveform.loadSoundfile("4chan.wav");
	timeline.setDurationInSeconds(waveform.getDuration());
    timeline.addBangs("clickbangs"); //just for fun

	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
    lastBang = 0;
}

//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
    lastBang = timeline.getCurrentTime();
}

//--------------------------------------------------------------
void testApp::update(){
	
	
}

//--------------------------------------------------------------
void testApp::draw(){
    float curtime = timeline.getCurrentTime();
	ofBackground(.15*255 * ofMap(curtime-lastBang, 0, .2, 1.0, 0., true));
    
	timeline.draw();
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
