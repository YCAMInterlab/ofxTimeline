#include "testApp.h"
#define BUTTON_HEIGHT 30

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
    
	timeline.setup();
    timeline.setFrameRate(30);
	timeline.setDurationInFrames(90);
	timeline.setLoopType(OF_LOOP_NORMAL);
	
	timeline.addKeyframes("brightness");
    timeline.addKeyframes("contrast");
    timeline.addKeyframes("saturation");
    timeline.addSwitcher("invert");
	
	timeline.getColors().load("defaultColors.xml");
	
    loaded = false;
	rendering = false;
    
    colorControl.load("colorcontrol");
    colorControl.begin();
    colorControl.setUniform1i("tex", 0);
    colorControl.end();
    
	if(settings.loadFile("settings.xml")){
        string videoPath = settings.getValue("videoPath", "");
        if(videoPath != ""){
            loadVideo(videoPath);
        }
    }
}

//--------------------------------------------------------------
void testApp::update(){
    
    if(loaded){
        //calculate a the view for the movie, scaled into the center between the timeline and the buttons
        float availableHeight = ofGetHeight() - timeline.getBottomLeft().y - BUTTON_HEIGHT;
        
        if(ofGetWidth() / availableHeight > contentRectangle.width/contentRectangle.height){
            outputRectangle.height = availableHeight;
            outputRectangle.width = contentRectangle.width * availableHeight / contentRectangle.height;
        }
        else {
            outputRectangle.width  = ofGetWidth();
            outputRectangle.height = contentRectangle.height * ofGetWidth()/contentRectangle.width;
        }
        outputRectangle.x =  ofGetWidth()/2 - outputRectangle.width/2;
        outputRectangle.y = timeline.getBottomLeft().y;
	}
    
    loadVideoButton = ofRectangle(0, ofGetHeight()-BUTTON_HEIGHT-1, ofGetWidth()/2, BUTTON_HEIGHT);
    renderButton = ofRectangle(ofGetWidth()/2, ofGetHeight()-BUTTON_HEIGHT-1, ofGetWidth()/2, BUTTON_HEIGHT);
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(.15*255);

    if(loaded){
		colorControl.begin();
        colorControl.setUniform1f("brightness", timeline.getKeyframeValue("brightness"));
        colorControl.setUniform1f("contrast", timeline.getKeyframeValue("contrast"));
        colorControl.setUniform1f("saturation", timeline.getKeyframeValue("saturation"));
        colorControl.setUniform1i("invert", (timeline.getSwitcherOn("invert") ? 1 : 0) );
    	timeline.getVideoTrack("Video")->getPlayer()->draw(outputRectangle);        
		colorControl.end();
    }
    
    ofPushStyle();
	ofNoFill();
    ofRect(loadVideoButton);
    ofRect(renderButton);
    ofPopStyle();
    
	timeline.draw();
    if(timeline.getIsPlaying()){
//        cout << timeline.getVideoTrack("Video")->getPlayer()->getTotalNumFrames()/timeline.getVideoTrack("Video")->getPlayer()->getDuration() << endl;
//        cout << timeline.getVideoTrack("Video")->getPlayer()->getPosition()*timeline.getVideoTrack("Video")->getPlayer()->getTotalNumFrames() << endl;
    }
}

//--------------------------------------------------------------
void testApp::loadVideo(string videoPath){
    ofxTLVideoTrack* videoTrack = timeline.getVideoTrack("Video");

    if(videoTrack == NULL){
	    videoTrack = timeline.addVideoTrack("Video", videoPath);
        loaded = (videoTrack != NULL);
    }
    else{
        loaded = videoTrack->load(videoPath);
    }

    if(loaded){
        contentRectangle = ofRectangle(0,0, videoTrack->getPlayer()->getWidth(), videoTrack->getPlayer()->getHeight());
        frameBuffer.allocate(contentRectangle.width, contentRectangle.height, GL_RGB);
        
        timeline.clear();
        //At the moment with video and audio tracks
        //ofxTimeline only works correctly if the duration of the track == the duration of the timeline
        //plan is to be able to fix this but for now...
        timeline.setDurationInFrames(videoTrack->getPlayer()->getTotalNumFrames());
        timeline.setTimecontrolTrack(videoTrack); //video playback will control the time
		timeline.bringTrackToTop(videoTrack);
    }
    else{
        videoPath = "";
    }
    settings.setValue("videoPath", videoPath);
    settings.saveFile();        
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' ' && loaded){
        timeline.togglePlay();
	}
    
	if(key == 'h'){
		timeline.toggleShow();
	}
    
    if(key == 'c'){
        colorControl.load("colorcontrol");
    }
}

//--------------------------------------------------------------
void testApp::startRender(){
	
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
    if(dragInfo.files.size() > 0){
		loadVideo(dragInfo.files[0]);
	}
}
	