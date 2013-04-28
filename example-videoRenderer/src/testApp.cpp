/**
 * Video Renderer Demo
 * ofxTimeline
 *
 * Shows how to use ofxTimeline to create video effects with a shader
 * that can be rendered to an image sequence
 */

#include "testApp.h"
#define BUTTON_HEIGHT 30

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
    ofEnableSmoothing();
	
    //set the timeline up with some default values
	timeline.setup();
    timeline.setFrameRate(30);
    //set big initial duration, longer than the video needs to be
	timeline.setDurationInFrames(20000);
	timeline.setLoopType(OF_LOOP_NORMAL);
    
    //add keyframes for  our shader
	timeline.addCurves("brightness", ofRange(0.0, 2.0), 1.0);
    timeline.addCurves("contrast", ofRange(.5, 2.0), 1.0);
    timeline.addCurves("saturation", ofRange(0.0, 1.5), 1.0);
    timeline.addSwitches("invert");
	
    loaded = false;
	rendering = false;
    renderFolder = "renders/"; //this is where rendered frames will be saved to
    
    //initialize the shader
    colorControl.load("shaders/colorcontrol");
    colorControl.begin();
    colorControl.setUniform1i("tex", 0);
    colorControl.end();
    
    //load the last video 
	if(settings.loadFile("settings.xml")){
        string videoPath = settings.getValue("videoPath", "");
        if(videoPath != ""){
            loadVideo(videoPath);
        }
    }
    
    //load our display font
    font.loadFont("GUI/NewMedia Fett.ttf", 15, true, true);
	font.setLineHeight(34.0f);
	font.setLetterSpacing(1.035);

}

//--------------------------------------------------------------
void testApp::update(){
    
    //update views
    if(!loaded){
        contentRectangle = ofRectangle(0,0, 16, 9); 
    }
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
    
    //loadVideoButton = ofRectangle(0, 0, ofGetWidth()/2, BUTTON_HEIGHT);
    renderButton = ofRectangle(outputRectangle.x, outputRectangle.y+outputRectangle.height, outputRectangle.width, BUTTON_HEIGHT-1);
    
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(.15*255); //pro apps background color

    if(loaded){
		colorControl.begin();
        colorControl.setUniform1f("brightness", timeline.getValue("brightness"));
        colorControl.setUniform1f("contrast", timeline.getValue("contrast"));
        colorControl.setUniform1f("saturation", timeline.getValue("saturation"));
        colorControl.setUniform1i("invert", (timeline.isSwitchOn("invert") ? 1 : 0) );
    	timeline.getVideoPlayer("Video")->draw(outputRectangle);        
		colorControl.end();
    }
    else{
        ofPushStyle();
        ofNoFill();
        ofSetColor(timeline.getColors().keyColor);
        ofRect(outputRectangle);
        string instructions = "Drag & Drop a Video file";
        float width = font.getStringBoundingBox("Drag & Drop a Video file or press load video", 0, 0).width;
        font.drawString(instructions, outputRectangle.x+outputRectangle.width/2 - width/2, outputRectangle.y + outputRectangle.height/2);
        ofPopStyle();
    }
    
    if(rendering){
        renderCurrentFrame();
    }
    
    ofPushStyle();
	ofNoFill();
    ofRect(loadVideoButton);
    if(loaded){
	    string renderString = rendering ? ("Cancel Render : " + ofToString(currentRenderFrame - timeline.getInFrame()) + "/" + ofToString(timeline.getOutFrame()-timeline.getInFrame()))  : "Start Render";
	    font.drawString(renderString, renderButton.x + 10, renderButton.y + renderButton.height*.75);
    }else{
    	font.drawString("load video", renderButton.x + 10, renderButton.y + renderButton.height*.75);
    }
    ofRect(renderButton);
    ofPopStyle();
    
	timeline.draw();    
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
        
        //timeline.clear();
        //At the moment with video and audio tracks
        //ofxTimeline only works correctly if the duration of the track == the duration of the timeline
        //plan is to be able to fix this but for now...
        timeline.setFrameRate(videoTrack->getPlayer()->getTotalNumFrames()/videoTrack->getPlayer()->getDuration());
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
void testApp::renderCurrentFrame(){
    
    //update the video and the timeline to the current frame
    int startFrame = timeline.getVideoPlayer("Video")->getCurrentFrame();
    //timeline.getVideoPlayer("Video")->setFrame(currentRenderFrame);
    timeline.getVideoPlayer("Video")->nextFrame();
    timeline.getVideoPlayer("Video")->update();
    int videoFrameToRender = timeline.getVideoPlayer("Video")->getCurrentFrame();
    float timeToSetTimeline = timeline.getVideoPlayer("Video")->getPosition() * timeline.getVideoPlayer("Video")->getDuration();
    timeline.setCurrentFrame(videoFrameToRender);
    
    //draw the video with the shader into the frame buffer
    frameBuffer.begin();
    ofClear(0,0,0);
    colorControl.begin();
    colorControl.setUniform1f("brightness", timeline.getValue("brightness"));
    colorControl.setUniform1f("contrast", timeline.getValue("contrast"));
    colorControl.setUniform1f("saturation", timeline.getValue("saturation"));
    colorControl.setUniform1i("invert", (timeline.isSwitchOn("invert") ? 1 : 0) );
    timeline.getVideoPlayer("Video")->draw(contentRectangle);//draw the frame buffer at full frame
    colorControl.end();
	frameBuffer.end();
    
    cout << "RENDERING -- Target Current Frame: " << currentRenderFrame << " start frame " << startFrame << " video frame (+1) " << videoFrameToRender << " video reports time " << timeToSetTimeline << " timeline difference " << (timeToSetTimeline - timeline.getCurrentTime()) << " frame " << timeline.getCurrentFrame() << endl;
    
    //save the image to file and update to the next frame
    ofImage saveImage;
    frameBuffer.readToPixels(saveImage.getPixelsRef());
    char filename[1024];
    sprintf(filename, "%s/frame_%05d.png",renderFolder.c_str(),currentRenderFrame);
    saveImage.saveImage(filename);
    currentRenderFrame++;
    if(currentRenderFrame > timeline.getOutFrame()){
        rendering = false;
        timeline.enable();
		timeline.setCurrentFrame(timeline.getInFrame());
    }
}

void testApp::keyPressed(int key){
    
    if(key == 'c'){
        //reload shader
        colorControl.load("colorcontrol");
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
	if(loaded && renderButton.inside(x,y)){
        if(rendering){
            rendering = false;
            timeline.enable();
        }
        else{
            //Make sure the render folder exists.
            //this will put all the frames into bin/data/renders
            ofDirectory renders(renderFolder);
            if(!renders.exists()){
                renders.create(true);
            }
			rendering = true;
            currentRenderFrame = timeline.getInFrame();
            timeline.getVideoPlayer("Video")->getPlayer()->setFrame(currentRenderFrame);
            timeline.stop();
            timeline.disable();
        }
    }else if(renderButton.inside(x,y)){
       	ofFileDialogResult result = ofSystemLoadDialog("select video",false);
       	if(result.bSuccess){
       		loadVideo(result.getPath());
       	}
    }
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
	
