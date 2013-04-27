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
#include "ofRange.h"
#include "ofxTLTrack.h"
#include "ofxXmlSettings.h"

class ofxTLKeyframe {
  public:
	ofVec2f screenPosition; // cached screen position
	unsigned long long previousTime; //for preventing overlap conflicts
    unsigned long long time; //in millis
    float value; //normalized
    long grabTimeOffset;
    float grabValueOffset;
};

class ofxTLKeyframes : public ofxTLTrack
{
  public:	
	ofxTLKeyframes();
	virtual ~ofxTLKeyframes();

	virtual void draw();
	
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
	virtual void nudgeBy(ofVec2f nudgePercent);
	
	virtual void keyPressed(ofKeyEventArgs& args);

	virtual void getSnappingPoints(set<unsigned long long>& points);
	
	virtual void save();
	virtual void load();
	
	virtual void clear();

	virtual void addKeyframe();
	virtual void addKeyframe(float value);
	virtual void addKeyframeAtMillis(unsigned long long millis);
	virtual void addKeyframeAtMillis(float value, unsigned long long millis);
	
    vector<ofxTLKeyframe*>& getKeyframes();
    
	//copy paste
	virtual string copyRequest();
	virtual string cutRequest();
	virtual void pasteSent(string pasteboard);
	virtual void selectAll();
	virtual void unselectAll();
    
    virtual int getSelectedItemCount();
    
	virtual unsigned long long getEarliestTime();
	virtual unsigned long long getLatestTime();
	virtual unsigned long long getEarliestSelectedTime();
	virtual unsigned long long getLatestSelectedTime();

    //undo
    virtual string getXMLRepresentation();
    virtual void loadFromXMLRepresentation(string rep);
    
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);
	
    virtual string getTrackType();
    
	//sampling
	virtual float getValue();
	virtual float getValueAtPercent(float percent);
	virtual float getValueAtTimeInMillis(long sampleTime);

	virtual void setValueRange(ofRange range, float defaultValue = 0);
	virtual void setValueRangeMin(float min);
	virtual void setValueRangeMax(float max);
	virtual void setDefaultValue(float defaultValue);
	
	virtual void quantizeKeys(int step);
	
    virtual ofRange getValueRange();
	
	//experimental binary saving. does not work with subclasses yet
	void saveToBinaryFile();
	void loadFromBinaryFile();
	bool useBinarySave;
	
  protected:
	virtual ofxTLKeyframe* newKeyframe();
	vector<ofxTLKeyframe*> keyframes;
	
	//cached previews for fast drawing of large timelines
	ofPolyline preview;
	vector<ofVec2f> keyPoints;
	
	virtual void recomputePreviews();
	bool shouldRecomputePreviews;
	
	virtual float sampleAtPercent(float percent); //less accurate than millis
    virtual float sampleAtTime(long sampleTime);
	virtual float interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long long sampleTime);
	virtual float evaluateKeyframeAtTime(ofxTLKeyframe* key, unsigned long long sampleTime, bool firstKey = false);

    ofRange valueRange;
	float defaultValue;
	
	//keep these stored for efficient search through the keyframe array
	int lastKeyframeIndex;
	unsigned long long lastSampleTime;
	
    virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
	bool isKeyframeIsInBounds(ofxTLKeyframe* key);
	bool isKeyframeSelected(ofxTLKeyframe* k);
    void selectKeyframe(ofxTLKeyframe* k);
    void deselectKeyframe(ofxTLKeyframe* k);
    
	//don't override these in subclasses
	void deleteSelectedKeyframes();
	void deleteKeyframe(ofxTLKeyframe* keyframe);
	//instead implement special behavior here:
	//this is called before the keyframe is deleted and removed from the keyframes vector
	virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe){};
	
	vector<ofxTLKeyframe*> selectedKeyframes;
    ofxTLKeyframe* selectedKeyframe;
	ofxTLKeyframe* hoverKeyframe;
	
	int selectedKeyframeIndex;
	bool keysAreDraggable;
	bool keysAreStretchable;
	unsigned long long stretchAnchor;
	unsigned long long stretchSelectPoint;
	
	virtual void setKeyframeTime(ofxTLKeyframe* key, unsigned long long newTime);
	virtual void updateKeyframeSort();
	virtual void updateStretchOffsets(ofVec2f screenpoint, long grabMillis);
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
	bool createNewOnMouseup;

};
