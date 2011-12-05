/*
 *  ofxTLPage.h
 *  timelineExample
 *
 *  Created by Jim on 10/2/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
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