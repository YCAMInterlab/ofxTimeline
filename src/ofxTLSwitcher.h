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

#pragma once 
#include "ofxTLKeyframer.h"

class ofxTLSwitch : public ofxTLKeyframe {
  public:
    //NOTE this does not use position, but copies everything into the range
	ofRange time;
	
	//ui stuff
    //	ofRange dragOffsets;
	bool startSelected;
	bool endSelected;
    ofRectangle display;
};

class ofxTLSwitcher : public ofxTLKeyframer {
  public:
	ofxTLSwitcher();
	~ofxTLSwitcher();

    virtual void draw();
    
    virtual bool isOn(float percent);
    
    void mouseDragged(ofMouseEventArgs& args, bool snapped);
    
  protected:
    virtual ofxTLKeyframe* newKeyframe(ofVec2f point);
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
    
    //a small distance that changes on the viewport
    float getSmallRange();
};