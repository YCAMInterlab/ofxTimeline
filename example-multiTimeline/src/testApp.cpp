/**
 * Multi timeline exmaple
 * ofxTimeline
 *
 * Examples of how to create simple interactions between multiple timelines
 */

#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(.15*255);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
    ofEnableSmoothing();
    
    //on mac lets you use COMMAND+C and COMMAND+V actions
	ofxTimeline::removeCocoaMenusFromGlut("MultiTimeline Example");
	
	timeline.setup();
	timeline.setDurationInSeconds(18);
    timeline.setLoopType(OF_LOOP_NORMAL);
    
    timeline.addFlags("StartStop");
    
    ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);

    for(int i = 0; i < 5; i++){
        ofxTimeline* t = new ofxTimeline();
		t->setup();
        t->setSpacebarTogglePlay(false);
        t->setDurationInFrames(400);
        t->addCurves("Sub Timeline " + ofToString(i));
        t->setWidth(ofGetWidth());
        t->setLoopType(OF_LOOP_NORMAL);
        sublines.push_back(t);	
    }
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(), 3), ofGetWidth()-100, ofGetHeight()-50);
    
    timeline.draw(); 

    sublines[0]->setOffset(timeline.getBottomLeft());

    //we need to offset all the timelines below the one above it, with a 10 pixel buffer
    for(int i = 0; i < sublines.size(); i++){
        if(i != 0){
            sublines[i]->setOffset(sublines[i-1]->getBottomLeft() + ofVec2f(0, 10));
        }
        sublines[i]->draw();
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    //reset the sub timelines when you hit play
	if(key ==  ' '){
        for(int i = 0; i < sublines.size(); i++){
            sublines[i]->stop();
            sublines[i]->setCurrentTimeSeconds(0);
        }            
    }
}

//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& bang){
    
    //This is our own custom flag protocol to enslave playback
    //on the sub timelines
    //a flag that is start 3 or stop 2 will start and stop those timelines

	vector<string> command = ofSplitString(bang.flag, " ");
    if(command.size() == 2){
        int trackIndex = ofToInt(command[1]);           
        if(trackIndex >= 0 && trackIndex < sublines.size()){
            if(command[0] == "start"){
                sublines[trackIndex]->play();
            }
            else if(command[0] == "stop"){
                sublines[trackIndex]->stop();
            }    
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
