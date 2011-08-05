/*
 *  ofxTLZoomer.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "ofxTLZoomer.h"
#include "ofxXmlSettings.h"
#include "ofRange.h"

ofxTLZoomer::ofxTLZoomer()
:	minSelected(false),
	maxSelected(false),
	midSelected(false),
	currentViewRange(ofRange(0.0, 1.0))
{
	//default constructor
}

void ofxTLZoomer::draw()
{
	ofPushStyle();
	ofEnableSmoothing();

	ofNoFill();
	if(focused){
		ofSetColor(255, 200, 0); //focused outline color
	}
	else{
		ofSetColor(150, 150, 0); //unfocused outline color
	}
	
	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);

	//draw min
	float screenY = bounds.y + bounds.height/2.0;
	float minScreenX = normalizedXtoScreenX(currentViewRange.min);
	float maxScreenX = normalizedXtoScreenX(currentViewRange.max);

	if(midSelected){
		ofSetLineWidth(2);
	}
	else{
		ofSetLineWidth(1);
	}

	ofLine(minScreenX, screenY, maxScreenX, screenY);
	ofSetLineWidth(1);

	if(minSelected){
		ofFill();
	}
	else{
		ofNoFill();
	}
	ofCircle(minScreenX, screenY, 5);

	if(maxSelected){
		ofFill();
	}
	else{
		ofNoFill();
	}
	ofCircle(maxScreenX, screenY, 5);

	ofPopStyle();
}

void ofxTLZoomer::load()
{
	ofxXmlSettings settings;
	if(!settings.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "ofxTLZoomer -- couldn't load zoom settings file");
		return;
	}

	settings.pushTag("zoom");
	currentViewRange = ofRange(settings.getValue("min", 0.0),
							   settings.getValue("max", 1.0));

	settings.popTag();
}

void ofxTLZoomer::save()
{
	ofxXmlSettings savedSettings;
	savedSettings.addTag("zoom");
	savedSettings.pushTag("zoom");
	savedSettings.addValue("min", currentViewRange.min);
	savedSettings.addValue("max", currentViewRange.max);
	savedSettings.popTag();//zoom
	savedSettings.saveFile(xmlFileName);
}

void ofxTLZoomer::mouseMoved(ofMouseEventArgs& args)
{
	
}

void ofxTLZoomer::mousePressed(ofMouseEventArgs& args)
{
	if(!enabled) return;

	minSelected = maxSelected = midSelected = focused  = false;
	if (pointInScreenBounds(ofVec2f(args.x, args.y))) {
		mouseIsDown = true;
		focused = true;
		float minScreenX = normalizedXtoScreenX(currentViewRange.min);
		minGrabOffset = args.x - minScreenX ;
		if(fabs(minScreenX - args.x) < 5){
			minSelected = true;
			return;
		}

		float maxScreenX = normalizedXtoScreenX(currentViewRange.max);
		maxGrabOffset = args.x - maxScreenX;
		if(fabs(maxScreenX - args.x) < 5){
			maxSelected = true;
			return;
		}

		if(args.x > minScreenX && args.x < maxScreenX){
			midSelected = true;
		}
	}
}

void ofxTLZoomer::mouseDragged(ofMouseEventArgs& args)
{
	if(!enabled) return;

	if(minSelected || midSelected){
		currentViewRange.min = ofClamp( screenXtoNormalizedX(args.x-minGrabOffset), 0, currentViewRange.max);
	}

	if(maxSelected || midSelected){
		currentViewRange.max = ofClamp( screenXtoNormalizedX(args.x-maxGrabOffset), currentViewRange.min, 1.0);
	}
}

bool ofxTLZoomer::isActive()
{
	return mouseIsDown && ( maxSelected || minSelected || midSelected);
}

void ofxTLZoomer::mouseReleased(ofMouseEventArgs& args)
{
	if(!enabled) return;

	mouseIsDown = false;
	if(autosave){
		save();
	}
}

void ofxTLZoomer::keyPressed(ofKeyEventArgs& args)
{
	//TODO: nudge
}

ofRange ofxTLZoomer::getViewRange()
{
	return currentViewRange;
}