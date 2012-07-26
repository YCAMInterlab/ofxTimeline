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

#include "ofxTLTrigger.h"
#include "ofxTimeline.h"

//bool triggersort(Trigger a, Trigger b){
//	return a.pt < b.pt;
//}

ofxTLTrigger::ofxTLTrigger(){
	enteringText = false;
//	selectedTrigger = hoverTrigger = NULL;
}

ofxTLTrigger::~ofxTLTrigger(){
}

//void ofxTLTrigger::setup(){
//	enable();
//	load();
//	
//	ofxTLRegisterPlaybackEvents(this);
//}

void ofxTLTrigger::draw(){
	
	if(bounds.height < 2){
		return;
	}
	
	ofPushStyle();
	
    ofxTLBangTrack::draw();
    
	ofFill();
	ofSetLineWidth(5);
	for(int i = keyframes.size()-1; i >= 0; i--){
        ofxTLTriggerKey* key = (ofxTLTriggerKey*)keyframes[i];
		int screenX = normalizedXtoScreenX(key->position.x, zoomBounds);

		ofSetColor(timeline->getColors().backgroundColor);		
		int textHeight = bounds.y + 10 + ( (20*i) % int(bounds.height) );
		key->display = ofRectangle(screenX+2.5, textHeight-10, 100, 15);
		ofRect(key->display);
        
		ofSetColor(timeline->getColors().textColor);		
		if(enteringText && isKeyframeSelected(key)){
			key->textField.draw(screenX, textHeight-10); //-10 accounts for textfield's offset
		}
		else {
			string timeString = timeline->formatTime(key->position.x);
			ofDrawBitmapString(key->textField.text, screenX+5, textHeight);
		}
	}
	ofPopStyle();
}

//main function to get values out of the timeline, operates on the given value range
void ofxTLTrigger::mousePressed(ofMouseEventArgs& args){
	
    bool clickedOnTextField = false;
    //look at each element to see if 
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLTriggerKey* key = (ofxTLTriggerKey*)keyframes[i];
		if(key->display.inside(args.x, args.y)){
            clickedOnTextField = true;
            if(!enteringText){
                if(!isKeyframeSelected(key)){
                    timeline->unselectAll();
                    selectedKeyframes.push_back(key);
                }
                enteringText = true;
                timeline->presentedModalContent(this);
                return;
            }
			break;
        }
    }
    
	if(enteringText && !clickedOnTextField){
        timeline->dismissedModalContent();
        enteringText = false;
        return;
    }
	
    ofxTLBangTrack::mousePressed(args);
    
//	enterText = false;
//	selectedTrigger = NULL;
//
//	if(hasFocus()){
//		selectedTrigger = getTriggerForScreenPosition(args.x, dragOffset);
//		if(selectedTrigger == NULL){
//			Trigger t;
//			t.pt = screenXtoNormalizedX(args.x, zoomBounds);
//			t.name = "new trigger";
//			triggers.push_back(t);
//			selectedTrigger = &triggers[triggers.size()-1];
//			dragOffset = 0;
//		}
//		timeline->setDragAnchor(dragOffset);
//	}
}


//void ofxTLTrigger::mouseMoved(ofMouseEventArgs& args){
//	ofxTLTrack::mouseMoved(args);	
//	int unused;
//	hoverTrigger = getTriggerForScreenPosition(args.x, unused);
//}

//void ofxTLTrigger::mouseDragged(ofMouseEventArgs& args, bool snapped){
////	ofxTLTrack::mouseDragged(args);
//	if(selectedTrigger != NULL){
//		selectedTrigger->pt = screenXtoNormalizedX(args.x - dragOffset, zoomBounds);
//	}
//	
//	//TODO: respect timeline->getMovePlayheadOnPaste()
//}

//void ofxTLTrigger::mouseReleased(ofMouseEventArgs& args){
//	ofxTLTrack::mouseReleased(args);	
//	if(selectedTrigger != NULL){
//		sortTriggers();
//		hoverTrigger = NULL;
//		if(autosave) save();
//	}
//}

void ofxTLTrigger::keyPressed(ofKeyEventArgs& args){
	
	if(enteringText){
        if(args.key == OF_KEY_RETURN){
            enteringText = false;
            timeline->dismissedModalContent();
        }
        else {
	        for(int i = 0; i < selectedKeyframes.size(); i++){
                ofxTLTriggerKey* key = (ofxTLTriggerKey*)selectedKeyframes[i];
				key->textField.keyPressed(args);
            }
        }
    }
	else{
        ofxTLBangTrack::keyPressed(args);
    }
    
    /*
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
     */
    
}

//void ofxTLTrigger::nudgeBy(ofVec2f nudgePercent){
//	if(selectedTrigger != NULL){
//		selectedTrigger->pt += nudgePercent.x;
//	}
//}

//void ofxTLTrigger::update(ofEventArgs& args){
//	float thisTimelinePoint = timeline->getPercentComplete();
//	for(int i = 0; i < triggers.size(); i++){
//		if(lastTimelinePoint < triggers[i].pt && thisTimelinePoint > triggers[i].pt){
//			ofxTLTriggerEventArgs args;
//			args.triggerName = triggers[i].name;
//			args.triggerGroupName = name;
//			ofNotifyEvent(ofxTLEvents.trigger, args);
//		}
//	}
//	lastTimelinePoint = thisTimelinePoint;
//}

//void ofxTLTrigger::playbackStarted(ofxTLPlaybackEventArgs& args){
//	lastTimelinePoint = timeline->getPercentComplete();
//	ofAddListener(ofEvents().update, this, &ofxTLTrigger::update);
//}
//
//void ofxTLTrigger::playbackEnded(ofxTLPlaybackEventArgs& args){
//	ofRemoveListener(ofEvents().update, this, &ofxTLTrigger::update);
//}
//
//void ofxTLTrigger::playbackLooped(ofxTLPlaybackEventArgs& args){
//	lastTimelinePoint = 0;
//}
//
//string ofxTLTrigger::copyRequest(){
//	return "";	
//}
//
//string ofxTLTrigger::cutRequest(){
//	return "";	
//}
//
//void ofxTLTrigger::pasteSent(string pasteboard){
//	
//}
//
//void ofxTLTrigger::selectAll(){
//	//TODO:
//}
//
//void ofxTLTrigger::unselectAll(){
//	//TODO:
//}

ofxTLKeyframe* ofxTLTrigger::newKeyframe(ofVec2f point){
	ofxTLTriggerKey* key = new ofxTLTriggerKey();
    key->position = point;
	return key;
}

void ofxTLTrigger::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTriggerKey* triggerKey = (ofxTLTriggerKey*)key;
    triggerKey->textField.text = xmlStore.getValue("name", "");
}

void ofxTLTrigger::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLTriggerKey* triggerKey = (ofxTLTriggerKey*)key;
    xmlStore.addValue("name", triggerKey->textField.text);
}

//void ofxTLTrigger::save(){
//	ofxXmlSettings save;
//	save.addTag("triggers");
//	save.pushTag("triggers");
//	int numTriggers = triggers.size();
//	for(int i = 0; i < numTriggers; i++){
//		save.addTag("trigger");
//		save.pushTag("trigger", i);
//		
//		save.addValue("point", triggers[i].pt);
//		save.addValue("name", triggers[i].name);
//		
//		save.popTag();
//	}
//	
//	save.popTag();
//	save.saveFile(xmlFileName);
//}
//
//void ofxTLTrigger::load(){
//	ofxXmlSettings save;
//	if(save.loadFile(xmlFileName)){
//		save.pushTag("triggers");
//		int numTriggers = save.getNumTags("trigger");
//		for(int i = 0; i < numTriggers; i++){
//			save.pushTag("trigger", i);
//			
//			Trigger t;
//			t.pt = save.getValue("point", .0);
//			t.name = save.getValue("name", "");
//			triggers.push_back(t);
//			
//			save.popTag();
//		}
//		save.popTag();
//	}
//	else{
//		ofLogError("ofxTLTrigger -- Couldn't load trigger file " + xmlFileName);
//	}
//}
//
//void ofxTLTrigger::sortTriggers(){
//	float selectedPt;
//	if(selectedTrigger != NULL){
//		selectedPt = selectedTrigger->pt;
//	}
//	sort(triggers.begin(), triggers.end(), triggersort);
//	
//	if(selectedTrigger != NULL){
//		for(int i = 0; i < triggers.size(); i++){
//			if(triggers[i].pt == selectedPt){
//				selectedTrigger = &triggers[i];
//				break;
//			}
//		}
//	}
//}
//
//void ofxTLTrigger::clear(){
//	triggers.clear();
//}
//
//Trigger* ofxTLTrigger::getTriggerForScreenPosition(float screenx, int& offset){
//	for(int i = 0; i < triggers.size(); i++){
//		offset = screenx - normalizedXtoScreenX(triggers[i].pt, zoomBounds);
//		if (abs(offset) < 10) {
//			return &triggers[i];
//		}
//	}
//	return NULL;
//}
