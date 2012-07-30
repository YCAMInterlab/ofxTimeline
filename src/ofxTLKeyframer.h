/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
 * http://jamesgeorge.org + http://flightphase.com
 * http://github.com/obviousjim + http://github.com/flightphase 
 *
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
#include "ofRange.h"
#include "ofxTLTrack.h"
#include "ofxXmlSettings.h"

class ofxTLKeyframe {
  public:
	ofVec2f position; // x is value, y is time, all 0 - 1.0	
	//ui interaction vars -- only set when dragging
	ofVec2f grabOffset; 
};

class ofxTLKeyframer : public ofxTLTrack
{
  public:	
	ofxTLKeyframer();
	~ofxTLKeyframer();

	virtual void draw();
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args, bool snapped);
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void keyPressed(ofKeyEventArgs& args);

	virtual void getSnappingPoints(vector<float>& points);
	
	virtual void save();
	virtual void load();
	
	virtual void reset();
	virtual void clear();
	
	//copy paste
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	virtual void unselectAll();
    
    virtual void regionSelected(ofRange timeRange, ofRange valueRange);
	
  protected:
	virtual ofxTLKeyframe* newKeyframe(ofVec2f point);

	vector<ofxTLKeyframe*> keyframes;

    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
	bool isKeyframeIsInBounds(ofxTLKeyframe* key);
	bool isKeyframeSelected(ofxTLKeyframe* k);
    void selectKeyframe(ofxTLKeyframe* k);
    void deselectKeyframe(ofxTLKeyframe* k);
    
	void nudgeSelectedKeyframes(ofVec2f nudge);
	void deleteSelectedKeyframes();
	
	vector<ofxTLKeyframe*> selectedKeyframes;
    ofxTLKeyframe* selectedKeyframe;
	ofxTLKeyframe* hoverKeyframe;
	
	int selectedKeyframeIndex;
	bool keysAreDraggable;
	
	virtual void updateKeyframeSort();
	virtual void updateDragOffsets(ofVec2f screenpoint);

	virtual string getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys);
	virtual void createKeyframesFromXML(ofxXmlSettings xml, vector<ofxTLKeyframe*>& keyContainer);
	virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){};
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){};

    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args){};

	bool screenpointIsInBounds(ofVec2f screenpoint);
	ofVec2f coordForKeyframePoint(ofVec2f keyframePoint);
	ofVec2f keyframePointForCoord(ofVec2f coord);
	
	bool keysDidDrag;
	bool keysDidNudge;
};
