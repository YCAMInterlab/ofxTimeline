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

#include "ofxTLFlags.h"
#include "ofxTimeline.h"
#include "ofxTLUtils.h"

ofxTLFlags::ofxTLFlags(){
	enteringText = false;
}

ofxTLFlags::~ofxTLFlags(){
}

void ofxTLFlags::draw(){
	
	if(bounds.height < 2){
		return;
	}
	
	ofPushStyle();
	
    ofxTLBangs::draw();
    
	ofFill();
	ofSetLineWidth(5);
	for(int i = keyframes.size()-1; i >= 0; i--){
        ofxTLFlag* key = (ofxTLFlag*)keyframes[i];
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
void ofxTLFlags::mousePressed(ofMouseEventArgs& args){
	
    if(!enteringText && ofGetModifierKeyShift()){
        ofxTLBangs::mousePressed(args);
        return;
    }
        
    ofxTLFlag* clickedTextField = NULL;
    
    //bool clickedOnTextField = false;
    //look at each element to see if 
    for(int i = 0; i < keyframes.size(); i++){
        ofxTLFlag* key = (ofxTLFlag*)keyframes[i];
		if(key->display.inside(args.x, args.y)){
            //clickedOnTextField = true;
            clickedTextField = key;
            break;
        }
    }

    if(clickedTextField != NULL){
        enteringText = true;
        timeline->presentedModalContent(this);
        if(!ofGetModifierKeyShift()){
            timeline->unselectAll();
        }
        if(!isKeyframeSelected(clickedTextField)){
            selectedKeyframes.push_back(clickedTextField);
        }
        return;
    }    
    else{        
        if(enteringText){
            enteringText = false;
            timeline->dismissedModalContent();
            timeline->unselectAll();
            return;
        }
    }
    
    ofxTLBangs::mousePressed(args);
}

void ofxTLFlags::mouseDragged(ofMouseEventArgs& args, bool snapped){
    if(!enteringText){
        ofxTLBangs::mouseDragged(args, snapped);
    }
}

void ofxTLFlags::keyPressed(ofKeyEventArgs& args){
	
	if(enteringText){
        if(args.key == OF_KEY_RETURN){
            enteringText = false;
            timeline->dismissedModalContent();
        }
        else {
	        for(int i = 0; i < selectedKeyframes.size(); i++){
                ofxTLFlag* key = (ofxTLFlag*)selectedKeyframes[i];
				key->textField.keyPressed(args);
            }
        }
    }
	else{
        ofxTLBangs::keyPressed(args);
    }    
}

ofxTLKeyframe* ofxTLFlags::newKeyframe(ofVec2f point){
	ofxTLFlag* key = new ofxTLFlag();
    key->position = point;
	return key;
}

void ofxTLFlags::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLFlag* triggerKey = (ofxTLFlag*)key;
    triggerKey->textField.text = xmlStore.getValue("flag", "");
}

void ofxTLFlags::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
    ofxTLFlag* triggerKey = (ofxTLFlag*)key;
    xmlStore.addValue("flag", triggerKey->textField.text);
}

void ofxTLFlags::bangFired(ofxTLKeyframe* key){
    ofxTLBangEventArgs args;
    args.trackName = name;
    args.flag = ((ofxTLFlag*)key)->textField.text;
    ofNotifyEvent(ofxTLEvents.bangFired, args);    
}

