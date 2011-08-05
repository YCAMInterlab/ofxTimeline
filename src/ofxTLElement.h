/*
 *  ofxTLElement.h
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxTween.h"
#include "ofRange.h"

class ofxTLElement 
{
  public:
	ofxTLElement();
	~ofxTLElement();
	
	virtual void draw() = 0;
	
	virtual void enable();
	virtual void disable();
	
	virtual bool hasFocus();
	
	virtual void setDrawRect(ofRectangle bounds);
	virtual void offsetDrawRect(ofVec2f offset);
	
	virtual ofRectangle getDrawRect();
	
	virtual void setZoomBounds(ofRange zoomBoundsPercent); //allows you to zoom in!
	
	virtual void setXMLFileName(string filename);
	virtual void setAutosave(bool autosave);
	
	virtual void mousePressed(ofMouseEventArgs& args){};
	virtual void mouseMoved(ofMouseEventArgs& args){};
	virtual void mouseDragged(ofMouseEventArgs& args){};
	virtual void mouseReleased(ofMouseEventArgs& args){};
	
	virtual void keyPressed(ofKeyEventArgs& args){};
	
	virtual void save(){};
	virtual void load(){};
	
	virtual void reset(){};
	virtual void clear(){};
	
  protected:
	
	virtual bool pointInScreenBounds(ofVec2f screenpoint);
	virtual float screenXtoNormalizedX(float x);
	virtual float normalizedXtoScreenX(float x);
	
	bool hover;
	bool enabled; //it's up to the implementation to respect this
	bool focused; //it's up to the implementation to modify this;
	bool autosave; //it's up to the implementation to save when it's modified based on this var
	
	ofxXmlSettings settings;
	string xmlFileName;
	ofRectangle bounds;
	ofRange zoomBounds;
	
};
