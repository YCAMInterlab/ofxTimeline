/*
 *  ofxTLPageTabs.h
 *  timelineExample
 *
 *  Created by Jim on 10/2/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"
#include "ofxTLEvents.h"

typedef struct{
	string name;
	ofRectangle bounds;
} Tab;

class ofxTLPageTabs : public ofxTLElement {

  public:
	
	virtual void setup();
	virtual void draw();

	virtual void addPage(string name);
	virtual void selectPage(string name);
	virtual void selectPage(int index);
	
	virtual void mouseReleased(ofMouseEventArgs& args);
	
	virtual void keyPressed(ofKeyEventArgs& args);
	
  protected:
	virtual void drawRectChanged();
	int selectedPageIndex;
	vector<Tab> pages;

	
};