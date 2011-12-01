/*
 *  ofxTLColors.cpp
 *  timelineExample
 *
 *  Created by James George on 11/30/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxTLColors.h"

void ofxTLColors::loadColors() {
	loadColors("../../../../../addons/ofxTimeline/assets/defaultColors.xml");
}

//TODO add reasonable default colors
void ofxTLColors::loadColors(string colorFile) {
	ofxXmlSettings settings;
	if(settings.loadFile(colorFile)){
		
		backgroundColor = ofColor(settings.getValue("colors:background:r", 0.0),
								  settings.getValue("colors:background:g", 0.0),
								  settings.getValue("colors:background:b", 0.0));
		
		keyColor = ofColor(settings.getValue("colors:key:r", 0.0),
						   settings.getValue("colors:key:g", 0.0),
						   settings.getValue("colors:key:b", 0.0));
		
		textColor = ofColor(settings.getValue("colors:text:r", 0.0),
						   settings.getValue("colors:text:g", 0.0),
						   settings.getValue("colors:text:b", 0.0));
		
		highlightColor = ofColor(settings.getValue("colors:highlight:r", 0.0),
								 settings.getValue("colors:highlight:g", 0.0),
								 settings.getValue("colors:highlith:b", 0.0));
		
		disabledColor = ofColor(settings.getValue("colors:disabled:r", 0.0),
								settings.getValue("colors:disabled:g", 0.0),
								settings.getValue("colors:disabled:b", 0.0));
		
		modalBackgroundColor = ofColor(settings.getValue("colors:modalBackground:r", 0.0),
								settings.getValue("colors:modalBackground:g", 0.0),
								settings.getValue("colors:modalBackground:b", 0.0));
		
		outlineColor = ofColor(settings.getValue("colors:outline:r", 0.0),
							   settings.getValue("colors:outline:g", 0.0),
							   settings.getValue("colors:outline:b", 0.0));
	}
	else{
		ofLogError("ofxTLColors -- Couldn't load color file " + colorFile);
	}
}
