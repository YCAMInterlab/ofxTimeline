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

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofxTLElementHeader.h"
#include "ofxTLEvents.h"

class ofxTLPage {
  public:
	
	ofxTLPage();
	~ofxTLPage();
	
	virtual void setup();
	virtual void draw();

	virtual void setName(string name);
	virtual string getName();
	virtual void setAutosave(bool doAutosave);
	
	virtual void setContainer(ofVec2f offset, float width);
	virtual void setHeaderHeight(float newHeaderHeight);
	virtual void setDefaultElementHeight(float newDefaultElementHeight);

	virtual float getComputedHeight();	
	
	virtual void addElement(string name, ofxTLElement* element);
	virtual ofxTLElement* getElement(string name);
	virtual void removeElement(string name);
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
		
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void saveElementPositions();
	virtual void loadElementPositions();
	
	virtual void recalculateHeight();
	
  protected:
	
	vector<ofxTLElementHeader*> headers;
	map<string, ofxTLElement*> elements;
	
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	bool isSetup;
	bool autosave;
	string name;
	
	ofVec2f containerOffset;
	float containerWidth;
	float computedHeight;
	float headerHeight;
	float defaultElementHeight;
	ofRange currentZoomBounds;

	map<string, ofRectangle> savedElementPositions;	
	
};