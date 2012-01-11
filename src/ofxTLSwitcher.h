/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
 * implementaiton by James George (@obviousjim) and Tim Gfrerer (@tgfrerer) for the 
 * Voyagers gallery National Maritime Museum 
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
#include "ofxTLElement.h"

typedef struct {
	ofRange time;
	
	//ui stuff
	ofRange dragOffsets;
	bool startSelected;
	bool endSelected;
} ofxTLSwitchOn;

class ofxTLSwitcher : public ofxTLElement
{
  public:
	ofxTLSwitcher();
	~ofxTLSwitcher();
	
	virtual void setup();
	virtual void draw();
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args,bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void save();
	virtual void load();
	
	virtual void clear();
	
	void playbackStarted(ofxTLPlaybackEventArgs& args);
	void playbackLooped(ofxTLPlaybackEventArgs& args);
	void playbackEnded(ofxTLPlaybackEventArgs& args);
	
	virtual bool isOn(float percent);
	
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	virtual void unselectAll();
	
	//Finds the closest switch behind a given point
	//if the point is inside a switch, it will return NULL.
	ofxTLSwitchOn* nearestSwitchBeforePoint(float percent);
	//Finds the closest switch after a given point
	ofxTLSwitchOn* nearestSwitchAfterPoint(float percent);
	
  protected:
	virtual void update(ofEventArgs& args);
	bool isSwitchInBounds(ofxTLSwitchOn* s);
	bool hoveringOn(float hoverY);
	
	void updateDragOffsets(float clickX);
//	void deselectAllSwitches();
	bool areAnySwitchesSelected();
	bool pointsAreDraggable;
	bool draggingSelectionRange;
	float selectionRangeAnchor;
	ofRange dragSelection;
	
	string getXMLStringForSwitches(bool selectedOnly);
	vector<ofxTLSwitchOn*> switchesFromXML(ofxXmlSettings xml);
	
	ofxTLSwitchOn* switchHandleForScreenX(float screenPos, bool& startTimeSelected);
	ofxTLSwitchOn* switchForScreenX(float screenPos);
	ofxTLSwitchOn* switchForPoint(float percent);
	
	vector<ofxTLSwitchOn*> switches;
	ofxTLSwitchOn* hoverSwitch;
	bool hoveringStartTime;
	bool hoveringHandle;
	

};
