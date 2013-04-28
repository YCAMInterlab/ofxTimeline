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
#include "ofxTLKeyframes.h"

class ofxTLSwitch : public ofxTLKeyframe {
  public:
    //NOTE this does not use time, but copies everything into the range
	ofLongRange timeRange;
	
	//ui stuff
    //	ofRange dragOffsets;
	bool startSelected;
	bool endSelected;
    long edgeDragOffset;
    ofRectangle display;
};

class ofxTLSwitches : public ofxTLKeyframes {
  public:
	ofxTLSwitches();
	virtual ~ofxTLSwitches();

    virtual void draw();

	virtual bool isOn();
    virtual bool isOnAtMillis(long millis);
    virtual bool isOnAtPercent(float percent);
    
    virtual bool mousePressed(ofMouseEventArgs& args, long millis);
    virtual void mouseDragged(ofMouseEventArgs& args, long millis);
    virtual void mouseReleased(ofMouseEventArgs& args, long millis);
    virtual void mouseMoved(ofMouseEventArgs& args, long millis);
    
    virtual void getSnappingPoints(set<unsigned long long>& points);
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);

    virtual void unselectAll();
    
    virtual string getTrackType();
    virtual void pasteSent(string pasteboard);
	
  protected:
    virtual ofxTLKeyframe* newKeyframe();
    virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
    virtual void updateEdgeDragOffsets(long clickMillis);
	virtual int getSelectedItemCount();
	virtual void nudgeBy(ofVec2f nudgePercent);

	//pushes any edits from keyframes superclass into the switches system
	virtual void updateTimeRanges();
	
    bool startHover;
    bool endHover;
    ofxTLSwitch* placingSwitch;
    
};