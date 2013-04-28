/**
 * Video Renderer Demo
 * ofxTimeline
 *
 * Shows how to use ofxTimeline to create video effects with a shader
 * that can be rendered to an image sequence
 */

#pragma once

#include "ofMain.h"
#include "ofxTimeline.h"

class testApp : public ofBaseApp{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

    void loadVideo(string videoPath);
    
    bool loaded;
    bool rendering;
    int currentRenderFrame;
    void renderCurrentFrame();
    
    string renderFolder;
    ofTrueTypeFont font;
    
    //saves between loads
    ofxXmlSettings settings;
    
	ofxTimeline timeline;
    //brightness,contrast,saturation shader found here: http://mouaif.wordpress.com/2009/01/05/photoshop-math-with-glsl-shaders/
    ofShader colorControl; 
    ofFbo frameBuffer;

    ofRectangle contentRectangle; //matched to video size
    ofRectangle outputRectangle;
    ofRectangle loadVideoButton;
    ofRectangle renderButton;
    
};
