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

class ofxTLTicker;
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
	
	virtual void collapseAllElements(); //collapses all element heights to 0;
	
	virtual void setZoomBounds(ofRange zoomBounds);
	
	virtual void unselectAll();
    virtual void reset();
    virtual void save();
    
	virtual float getComputedHeight();	
	
	virtual void addElement(string name, ofxTLElement* element);
	virtual ofxTLElement* getElement(string name);
	virtual void removeElement(string name);
    
    //given a folder the page will look for xml files to load within that
	virtual void loadElementsFromFolder(string folderPath);
    
	virtual void setSnapping(bool snapping);
	virtual void enableSnapToOtherElements(bool snapToOthes);
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
		
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void saveElementPositions();
	virtual void loadElementPositions();
	
	virtual void recalculateHeight();
	
	virtual void setTicker(ofxTLTicker* ticker);
	
	//copy paste
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	
	virtual void setDragAnchor(float anchor);
	
  protected:
	//used for getting BPM snaps
	ofxTLTicker* ticker;
	
	bool draggingInside;
	vector<ofxTLElementHeader*> headers;
	map<string, ofxTLElement*> elements;

	bool snappingEnabled;
	bool snapToOtherElementsEnabled;
	
	bool headerHasFocus;
	
	vector<float> snapPoints;
	//in pixels. TODO: make variable through API
	float snappingTolerance;
	virtual void zoomEnded(ofxTLZoomEventArgs& args);
	
	void refreshSnapPoints();
	
	float dragAnchor;
	
	bool isSetup;
	bool autosave;
	string name;
	
	ofRectangle elementContainerRect;
	float headerHeight;
	float defaultElementHeight;
	ofRange currentZoomBounds;

	map<string, ofRectangle> savedElementPositions;	
	
};