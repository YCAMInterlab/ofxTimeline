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
#include "ofxTLTrack.h"
#include "ofxTextInputField.h"

#define FOOTER_HEIGHT 6

class ofxTLTrackHeader : public ofxTLTrack
{
  public:	
	ofxTLTrackHeader();
	virtual ~ofxTLTrackHeader();
	
    string name;
    
    virtual void enable();
    virtual void disable();
    
	virtual void draw();
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void setTrack(ofxTLTrack* track);
	virtual ofxTLTrack* getTrack();
    
	virtual void setFooterHeight(float height);
	virtual void collapseTrack();
    
	virtual ofRectangle getFooterRect();
	virtual void textFieldEnter(string& newText);
	
	//for the header, this returns the track's display name
    virtual string getDisplayName();
	
  protected:
	ofxTLTrack* track;
	ofxTextInputField nameField;
	
	float footerHeight;
	void recalculateFooter();
	
	ofRectangle footerRect;
	
	bool hoveringFooter;
	bool draggingSize;
	float dragOffset;
	float dragAnchor;
	
	void recalculateFooterStripes();
	float footerStripeWidth;
	ofPath footerStripes;
	
};

