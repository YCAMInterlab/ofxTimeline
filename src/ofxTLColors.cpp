/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxTimeline 
 * Lightweight SDK for creating graphic timeline tools in openFrameworks
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
