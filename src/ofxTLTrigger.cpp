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

#include "ofxTLTrigger.h"
#include "ofxTimeline.h"

bool triggersort(Trigger a, Trigger b){
	return a.pt < b.pt;
}

ofxTLTrigger::ofxTLTrigger(){
	enterText = false;
	selectedTrigger = hoverTrigger = NULL;
}

ofxTLTrigger::~ofxTLTrigger(){
}

void ofxTLTrigger::setup(){
	enable();
	load();
	
	ofxTLRegisterPlaybackEvents(this);
}

void ofxTLTrigger::draw(){
	
	if(bounds.height < 2){
		return;
	}
	
	ofPushStyle();
	ofNoFill();
	if(hover){
		ofSetColor(timeline->getColors().highlightColor);
	}
	else if(focused){
		ofSetColor(timeline->getColors().highlightColor);
	}
	else{
		ofSetColor(timeline->getColors().outlineColor);
	}
	
	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);
	
	ofFill();
	ofSetLineWidth(5);
	for(int i = triggers.size()-1; i >= 0; i--){
		int screenX = normalizedXtoScreenX(triggers[i].pt, zoomBounds);
		if(&triggers[i] == selectedTrigger){
			ofSetColor(timeline->getColors().textColor);
		}
		else if(&triggers[i] == hoverTrigger){
			ofSetColor(timeline->getColors().highlightColor);
		}
		else{
			ofSetColor(timeline->getColors().keyColor);
		}

		int textHeight = bounds.y + 10 + ( (20*i) % int(bounds.height) );
		ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
		ofSetColor(timeline->getColors().backgroundColor);
		
		ofRect(screenX+2.5, textHeight-10, 100, 15);

		ofSetColor(timeline->getColors().textColor);
		if(enterText && &triggers[i] == selectedTrigger){
			textfield.draw(screenX, textHeight-10); //-10 accounts for textfield's offset
		}
		else {
			string timeString = timeline->getIsFrameBased() ? 
				ofToString(indexForScreenX(screenX)) : 
				ofToString(triggers[i].pt * timeline->getDurationInSeconds());
			
			ofDrawBitmapString(timeString+"|" +triggers[i].name, screenX+5, textHeight);
		}
	}
	ofPopStyle();
}

//main function to get values out of the timeline, operates on the given value range
void ofxTLTrigger::mousePressed(ofMouseEventArgs& args){

	enterText = false;
	selectedTrigger = NULL;

	bool clickIsInRect = bounds.inside(args.x, args.y);
	if(clickIsInRect){
		if(!focused){
			focused = true;
			return;
		}
	}
	else{
		focused = false;
		return;
	}
	
	if(focused){
		selectedTrigger = getTriggerForScreenPosition(args.x, dragOffset);
		if(selectedTrigger == NULL){
			Trigger t;
			t.pt = screenXtoNormalizedX(args.x, zoomBounds);
			t.name = "new trigger";
			triggers.push_back(t);
			selectedTrigger = &triggers[triggers.size()-1];
			dragOffset = 0;
		}
		timeline->setDragAnchor(dragOffset);
	}
}

void ofxTLTrigger::mouseMoved(ofMouseEventArgs& args){
	ofxTLElement::mouseMoved(args);	
	int unused;
	hoverTrigger = getTriggerForScreenPosition(args.x, unused);
}

void ofxTLTrigger::mouseDragged(ofMouseEventArgs& args, bool snapped){
//	ofxTLElement::mouseDragged(args);
	if(selectedTrigger != NULL){
		selectedTrigger->pt = screenXtoNormalizedX(args.x - dragOffset, zoomBounds);
	}
	
	//TODO respect timeline->getMovePlayheadOnPaste()
}

void ofxTLTrigger::mouseReleased(ofMouseEventArgs& args){
	ofxTLElement::mouseReleased(args);	
	if(selectedTrigger != NULL){
		sortTriggers();
		hoverTrigger = NULL;
		if(autosave) save();
	}
}

void ofxTLTrigger::keyPressed(ofKeyEventArgs& args){
	
	
	if((args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE) && selectedTrigger != NULL && !enterText){
		for(int i = triggers.size()-1; i>=0; i--){
			if(&triggers[i] == selectedTrigger){
				triggers.erase(triggers.begin()+i);
				selectedTrigger = NULL;
				if(autosave) save();
			}
		}
	}
		
	if(args.key == OF_KEY_RETURN && selectedTrigger != NULL){
		if(enterText){
			enterText = false;
			selectedTrigger->name = textfield.text;
			textfield.disable();
			if(autosave) save();
		}
		else {
			enterText = true;
			textfield.enable();
			textfield.cursorPosition = 0;
			if(selectedTrigger){
				if(selectedTrigger->name == "new trigger"){
					textfield.text = "";
				}
				else{
					textfield.text = selectedTrigger->name;
				}
			}
		}
	}
}

void ofxTLTrigger::nudgeBy(ofVec2f nudgePercent){
	if(selectedTrigger != NULL){
		selectedTrigger->pt += nudgePercent.x;
	}
}

void ofxTLTrigger::update(ofEventArgs& args){
	float thisTimelinePoint = timeline->getPercentComplete();
	for(int i = 0; i < triggers.size(); i++){
		if(lastTimelinePoint < triggers[i].pt && thisTimelinePoint > triggers[i].pt){
			ofxTLTriggerEventArgs args;
			args.triggerName = triggers[i].name;
			args.triggerGroupName = name;
			ofNotifyEvent(ofxTLEvents.trigger, args);
		}
	}
	lastTimelinePoint = thisTimelinePoint;
}

void ofxTLTrigger::playbackStarted(ofxTLPlaybackEventArgs& args){
	lastTimelinePoint = timeline->getPercentComplete();
	ofAddListener(ofEvents.update, this, &ofxTLTrigger::update);
}

void ofxTLTrigger::playbackEnded(ofxTLPlaybackEventArgs& args){
	ofRemoveListener(ofEvents.update, this, &ofxTLTrigger::update);
}

void ofxTLTrigger::playbackLooped(ofxTLPlaybackEventArgs& args){
	cout << "playback looped" << endl;
	lastTimelinePoint = 0;
}

string ofxTLTrigger::copyRequest(){
	return "";	
}

string ofxTLTrigger::cutRequest(){
	return "";	
}

void ofxTLTrigger::pasteSent(string pasteboard){
	
}

void ofxTLTrigger::selectAll(){
	//TODO:
}

void ofxTLTrigger::unselectAll(){
	//TODO:
}

void ofxTLTrigger::save(){
	ofxXmlSettings save;
	save.addTag("triggers");
	save.pushTag("triggers");
	int numTriggers = triggers.size();
	for(int i = 0; i < numTriggers; i++){
		save.addTag("trigger");
		save.pushTag("trigger", i);
		
		save.addValue("point", triggers[i].pt);
		save.addValue("name", triggers[i].name);
		
		save.popTag();
	}
	
	save.popTag();
	save.saveFile(xmlFileName);
}

void ofxTLTrigger::load(){
	ofxXmlSettings save;
	if(save.loadFile(xmlFileName)){
		save.pushTag("triggers");
		int numTriggers = save.getNumTags("trigger");
		for(int i = 0; i < numTriggers; i++){
			save.pushTag("trigger", i);
			
			Trigger t;
			t.pt = save.getValue("point", .0);
			t.name = save.getValue("name", "");
			triggers.push_back(t);
			
			save.popTag();
		}
		save.popTag();
	}
	else{
		ofLogError("ofxTLTrigger -- Couldn't load trigger file " + xmlFileName);
	}
}

void ofxTLTrigger::sortTriggers(){
	float selectedPt;
	if(selectedTrigger != NULL){
		selectedPt = selectedTrigger->pt;
	}
	sort(triggers.begin(), triggers.end(), triggersort);
	
	if(selectedTrigger != NULL){
		for(int i = 0; i < triggers.size(); i++){
			if(triggers[i].pt == selectedPt){
				selectedTrigger = &triggers[i];
				break;
			}
		}
	}
}

void ofxTLTrigger::clear(){
	triggers.clear();
}

Trigger* ofxTLTrigger::getTriggerForScreenPosition(float screenx, int& offset){
	for(int i = 0; i < triggers.size(); i++){
		offset = screenx - normalizedXtoScreenX(triggers[i].pt, zoomBounds);
		if (abs(offset) < 10) {
			return &triggers[i];
		}
	}
	return NULL;
}
