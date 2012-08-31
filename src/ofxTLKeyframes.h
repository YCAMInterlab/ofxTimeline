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
	ofVec2f screenPosition; // cached screen position
    long time; //in millis
    float value; //normalized
    long grabTimeOffset;
    float grabValueOffset;
    
//	//ui interaction vars -- only set when dragging
//	ofVec2f grabOffset; 
//    ofVec2f position;
};

class ofxTLKeyframes : public ofxTLTrack
{
  public:	
	ofxTLKeyframes();
	virtual ~ofxTLKeyframes();

	virtual void draw();
	
	virtual void mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void keyPressed(ofKeyEventArgs& args);

	virtual void getSnappingPoints(set<long>& points);
	
	virtual void save();
	virtual void load();
	
	virtual void clear();
	
	virtual void addKeyframeAtMillis(float value, unsigned long millis);
	virtual void addKeyframe(float value);
	
	//copy paste
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	virtual void unselectAll();
    
    virtual int getSelectedItemCount();
    
    //undo
    virtual string getXMLRepresentation();
    virtual void loadFromXMLRepresentation(string rep);
    
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);
	
    virtual string getTrackType();
    
	//sampling
	virtual float getValueAtPercent(float percent);
	virtual float getValueAtTimeInMillis(long sampleTime);

	virtual void setValueRange(ofRange range, float defaultValue = 0);
    virtual ofRange getValueRange();

  protected:
	virtual ofxTLKeyframe* newKeyframe();
	vector<ofxTLKeyframe*> keyframes;
	
	ofPolyline preview;
	vector<ofVec2f> keyPoints;
	void recomputePreviews();
	bool shouldRecomputePreviews;
	
	virtual float sampleAtPercent(float percent); //less accurate than millis
    virtual float sampleAtTime(long sampleTime);
	virtual float interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long sampleTime);
	
    ofRange valueRange;
	float defaultValue;
	
	//keep these stored for efficient search through the keyframe array
	int lastKeyframeIndex;
	unsigned long lastSampleTime;
	
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
	virtual void updateDragOffsets(ofVec2f screenpoint, long grabMillis);

	virtual string getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys);
	virtual void createKeyframesFromXML(ofxXmlSettings xml, vector<ofxTLKeyframe*>& keyContainer);
	virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){};
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){};

    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args){};

	bool screenpointIsInBounds(ofVec2f screenpoint);
	ofVec2f screenPositionForKeyframe(ofxTLKeyframe* keyframe);

    float screenYToValue(float screenY);
	float valueToScreenY(float value);
	
	bool keysDidDrag;
	bool keysDidNudge;
};
