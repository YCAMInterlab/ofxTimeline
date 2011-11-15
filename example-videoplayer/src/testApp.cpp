#include "testApp.h"
#include "ofxTimeline.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	timeline = new ofxTimeline();
	timeline->setup();

	playerElement = NULL;

	timeline->setDuration(300);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(.15*255);
	
	timeline->draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(playerElement == NULL){
		ofFileDialogResult r = ofSystemLoadDialog("Load Video File", false);
		if(r.bSuccess && player.loadMovie(r.filePath)){
			playerElement = new ofxTLVideoPlayer();
			playerElement->setup();
			timeline->setDuration(player.getTotalNumFrames());
			timeline->addElement("Video", playerElement);
			playerElement->setVideoPlayer(player, ofFilePath::removeExt( r.filePath ) + "/thumbs");
		}
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
	timeline->setWidth(w);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
