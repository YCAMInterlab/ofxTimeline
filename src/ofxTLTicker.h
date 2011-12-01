/*
 *  ofxTLTicker.h
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTLElement.h"

class ofxTLTicker : public ofxTLElement
{
  public:
	ofxTLTicker();
	~ofxTLTicker();
	
	virtual void setup();
	virtual void draw();
	
	//set the draw rect for the whole keyframer interface
	virtual void setTotalDrawRect(ofRectangle drawRect);
	
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);

  protected:
	virtual void updateHover(ofMouseEventArgs& args);
	
	ofRectangle totalDrawRect;
		
	float curHoverPoint;
	int curStartFrame;
	int curEndFrame;
	int curHoverFrame;
	int framesInView;
	bool hovering;

	float mousex;
};
