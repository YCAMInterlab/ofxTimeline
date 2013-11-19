/**
 * ofxTimeline
 * openFrameworks graphical timeline addon
 *
 * Copyright (c) 2011-2012 James George
 * Development Supported by YCAM InterLab http://interlab.ycam.jp/en/
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
 */

#pragma once
#include "ofMain.h"
#include "ofxTLBangs.h"
#include "ofxTextInputField.h"

class ofxTLFlag : public ofxTLKeyframe {
  public:
    ofxTextInputField textField;
    ofRectangle display;
//	virtual ~ofxTLFlag();
};

class ofxTLFlags : public ofxTLBangs {
  public:
	ofxTLFlags();
	
	virtual void draw();
	
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	virtual void unselectAll();
    
    virtual string getTrackType();
	
	virtual void addFlag(string key);
	virtual void addFlagAtTime(string key, unsigned long long time);
    virtual ofxTLFlag* getFlagWithKey(string key);
	
protected:
    
    virtual ofxTLKeyframe* newKeyframe();
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
    virtual void bangFired(ofxTLKeyframe* key);
	virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe);

	//only set per mousedown/mouseup cycle
	ofxTLFlag* clickedTextField;
	bool enteringText;

};