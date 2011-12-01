/*
 *  ofxTLColors.h
 *  timelineExample
 *
 *  Created by James George on 11/30/11.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxTLColors
{
  public:
	
	void loadColors(); //defaults
	void loadColors(string colorFile);
	
	ofColor backgroundColor;
	ofColor keyColor;
	ofColor textColor;
	ofColor highlightColor;
	ofColor disabledColor;
	ofColor modalBackgroundColor;
	ofColor outlineColor;
};