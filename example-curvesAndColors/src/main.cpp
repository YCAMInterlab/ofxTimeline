/**
 * Curves demo
 * ofxTimeline
 *
 * Shows how to use ofxTimeline to create a simple curves and color change
 */

#include "ofApp.h"

//========================================================================
int main( ){

	ofGLWindowSettings settings;
	settings.width = 1024;
	settings.height = 768;
	ofCreateWindow(settings);

	ofRunApp(new ofApp());

}
