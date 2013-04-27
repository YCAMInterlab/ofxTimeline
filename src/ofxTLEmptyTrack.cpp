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

#include "ofxTLEmptyTrack.h"
#include "ofxTimeline.h"

ofxTLEmptyTrack::ofxTLEmptyTrack(){
	
}

ofxTLEmptyTrack::~ofxTLEmptyTrack(){
	
}

//enable and disable are always automatically called
//in setup. Must call superclass's method as well as doing your own
//enabling and disabling
void ofxTLEmptyTrack::enable(){
	ofxTLTrack::enable();
	
	//other enabling
}

void ofxTLEmptyTrack::disable(){
	ofxTLTrack::disable();
	
	//other disabling
}

//update is called every frame.
//if your track triggers events it's good to do it here
//if timeline is set to thread this is called on the back thread so
//be careful if loading images in herre
void ofxTLEmptyTrack::update(){
	
}

//draw your track contents. use ofRectangle bounds to know where to draw
//and the Track functions screenXToMillis() or millisToScreenX() to respect zoom
void ofxTLEmptyTrack::draw(){
	
	//this is just a simple example
	ofPushStyle();
	ofFill();
	if(isHovering()){
		ofSetColor(timeline->getColors().backgroundColor);
		ofRect(bounds);
	}
	
	ofNoFill();
	ofSetColor(timeline->getColors().keyColor);
	for(int i = 0; i < clickPoints.size(); i++){
		float screenX = millisToScreenX(clickPoints[i].time);
		if(screenX > bounds.x && screenX < bounds.x+bounds.width){
			float screenY = ofMap(clickPoints[i].value, 0.0, 1.0, bounds.getMinY(), bounds.getMaxY());
			ofCircle(screenX, screenY, 4);
		}
	}
}

//caled by the timeline, don't need to register events
bool ofxTLEmptyTrack::mousePressed(ofMouseEventArgs& args, long millis){
	createNewPoint = isActive();
	clickPoint = ofVec2f(args.x,args.y);
	return createNewPoint; //signals that the click made a selection
}

void ofxTLEmptyTrack::mouseMoved(ofMouseEventArgs& args, long millis){
	
}
void ofxTLEmptyTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	
}
void ofxTLEmptyTrack::mouseReleased(ofMouseEventArgs& args, long millis){
	
	//need to create clicks on mouse up if the mouse hasn't moved in order to work
	//well with the click-drag rectangle thing
	if(createNewPoint && clickPoint.distance(ofVec2f(args.x, args.y)) < 4){
		ClickPoint newpoint;
		newpoint.value = ofMap(args.y, bounds.getMinY(), bounds.getMaxY(), 0, 1.0);
		newpoint.time = millis;
		clickPoints.push_back(newpoint);
		//call this on mouseup or keypressed after a click 
		//will trigger save and needed for undo
		timeline->flagTrackModified(this);
	}
}

//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
void ofxTLEmptyTrack::keyPressed(ofKeyEventArgs& args){
	
}
void ofxTLEmptyTrack::nudgeBy(ofVec2f nudgePercent){
	
}

//if your track has some selectable elements you can interface with snapping
//and selection/unselection here
void ofxTLEmptyTrack::getSnappingPoints(set<unsigned long long>& points){
	
}
void ofxTLEmptyTrack::regionSelected(ofLongRange timeRange, ofRange valueRange){
	
}
void ofxTLEmptyTrack::unselectAll(){
	
}
void ofxTLEmptyTrack::selectAll(){
	
}

//return a unique name for your track
string ofxTLEmptyTrack::getTrackType(){
	return "EmptyTrack";
}

//for copy+paste you can optionaly implement ways
//of creating XML strings that represent your selected tracks
string ofxTLEmptyTrack::copyRequest(){
	return "";
}

string ofxTLEmptyTrack::cutRequest(){
	return "";
}

//will return the same type of strings you provide in copy and paste
//but may contain foreign data from other tracks so be careful
void ofxTLEmptyTrack::pasteSent(string pasteboard){
	
}

//for undo and redo you can implement a way of
//reperesnt your whole track as XML
string ofxTLEmptyTrack::getXMLRepresentation(){
	return "";
}

void ofxTLEmptyTrack::loadFromXMLRepresentation(string rep){

}

//serialize your track.
//use ofxTLTrack's string xmlFileName
void ofxTLEmptyTrack::save(){
	
}

void ofxTLEmptyTrack::load(){
	
}

void ofxTLEmptyTrack::clear(){
	
}