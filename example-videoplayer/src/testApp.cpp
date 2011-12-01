#include "testApp.h"
#include "ofxTimeline.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	timeline.setup();
	timeline.setOffset(ofVec2f(0, 480));
	
	player.loadMovie("synchronizedswim.mp4");
	
	playerElement.setup();
	timeline.setDurationInFrames(player.getTotalNumFrames());
	timeline.addElement("Video", &playerElement);
	playerElement.setVideoPlayer(player, "video_thumbs");
	
	//set player rectangle 
	playerRect = ofRectangle(0,0,480 * player.getWidth()/player.getHeight(), 480);
	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(.15*255);
	if (player.isLoaded()) {
		player.draw(playerRect);
	}
	
	timeline.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(player.isPlaying()){
		player.stop();
	}
	else {
		player.play();
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
