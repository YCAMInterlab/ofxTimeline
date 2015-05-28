/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George http://www.jamesgeorge.org
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/ +
 *
 * http://github.com/YCAMinterLab
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
 */

#include "ofxTLColors.h"

void ofxTLColors::load() {
    string colorFile = "GUI/defaultColors.xml";
	load(colorFile);
}

void ofxTLColors::load(string colorFile) {
	ofxXmlSettings settings;
	if(!settings.loadFile( colorFile )){
	//if(!settings.loadFile( ofToDataPath(colorFile, true) )){
        ofLogWarning("ofxTLColors  -- Couldn't load color file " + colorFile );
    }
		
    guiBackgroundColor = ofColor(settings.getValue("colors:guiBackground:r", 0),
                                 settings.getValue("colors:guiBackground:g", 0),
                                 settings.getValue("colors:guiBackground:b", 0),
                                 settings.getValue("colors:guiBackground:a", 0));
    
    backgroundColor = ofColor(settings.getValue("colors:background:r", 41),
                              settings.getValue("colors:background:g", 42),
                              settings.getValue("colors:background:b", 53),
                              settings.getValue("colors:background:a", 255));

    textColor = ofColor(settings.getValue("colors:text:r", 255),
                        settings.getValue("colors:text:g", 255),
                        settings.getValue("colors:text:b", 255),
                        settings.getValue("colors:text:a", 255));

    keyColor = ofColor(settings.getValue("colors:key:r", 52),
                       settings.getValue("colors:key:g", 175),
                       settings.getValue("colors:key:b", 195),
                       settings.getValue("colors:key:a", 255));
    
    
    highlightColor = ofColor(settings.getValue("colors:highlight:r", 165),
                             settings.getValue("colors:highlight:g", 54),
                             settings.getValue("colors:highlight:b", 71),
                             settings.getValue("colors:highlight:a", 255));
    
    disabledColor = ofColor(settings.getValue("colors:disabled:r", 98),
                            settings.getValue("colors:disabled:g", 98),
                            settings.getValue("colors:disabled:b", 103),
                            settings.getValue("colors:disabled:a", 255));
    
    modalBackgroundColor = ofColor(settings.getValue("colors:modalBackground:r", 98),
                                   settings.getValue("colors:modalBackground:g", 98),
                                   settings.getValue("colors:modalBackground:b", 103),
                                   settings.getValue("colors:modalBackground:b", 255));
    
    outlineColor = ofColor(settings.getValue("colors:outline:r", 149),
                           settings.getValue("colors:outline:g", 204),
                           settings.getValue("colors:outline:b", 103),
                           settings.getValue("colors:outline:a", 255));

}
