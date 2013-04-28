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

#include "ofxTLInOut.h"
#include "ofxTimeline.h"

ofxTLInOut::ofxTLInOut()
 : ofxTLTrack(),
	dragOffset(0), 
	draggingIn(false), 
    draggingOut(false),
	hoveringIn(false),
	hoveringOut(false)
{
    
}

void ofxTLInOut::draw(){
    ofPushStyle();
    
	ofRange screenXRange(bounds.getMinX(),bounds.getMaxX());
    if(bounds.height > 2){
        ofSetLineWidth(3);
        int inScreenX = normalizedXtoScreenX( timeline->getInOutRange().min );
        int outScreenX = normalizedXtoScreenX( timeline->getInOutRange().max ); 
        if(screenXRange.contains(inScreenX)){
            if(hoveringIn){
                ofSetColor(timeline->getColors().highlightColor);
            }
            else{
                ofSetColor(timeline->getColors().keyColor);
            }
            ofLine(inScreenX, bounds.y, inScreenX, bounds.y+bounds.height);
        }

        if(screenXRange.contains(outScreenX)){
            if(hoveringOut){
                ofSetColor(timeline->getColors().highlightColor);
            }
            else{
                ofSetColor(timeline->getColors().keyColor);
            }
            ofLine(outScreenX, bounds.y, outScreenX, bounds.y+bounds.height);
        }
    }
    
    //draw inout over the whole thing
    ofFill();
    ofSetLineWidth(1);
	//draw in/out point
	float inPointX = ofClamp(normalizedXtoScreenX(timeline->getInOutRange().min), screenXRange.min, screenXRange.max);
	float outPointX = ofClamp(normalizedXtoScreenX(timeline->getInOutRange().max),screenXRange.min, screenXRange.max);
    
	if(bounds.x < inPointX){
		ofSetColor(timeline->getColors().disabledColor,120);
		ofRect(bounds.x, pageRect.y, inPointX - bounds.x, pageRect.height);
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(inPointX, pageRect.y, inPointX, pageRect.y+pageRect.height);
	}
	
	if(bounds.x+bounds.width > outPointX){
		ofSetColor(timeline->getColors().disabledColor,120);
		ofRect(outPointX, pageRect.y, (bounds.x+bounds.width) - outPointX, pageRect.height);	
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(outPointX, pageRect.y, outPointX, pageRect.y+pageRect.height);
	}
	
	ofPopStyle();

}

void ofxTLInOut::setPageRectangle(ofRectangle pageRectangle){
    pageRect = pageRectangle;
}

void ofxTLInOut::mouseMoved(ofMouseEventArgs& args){

    if(!bounds.inside(args.x,args.y)) return;
    
    hoveringIn  = abs( normalizedXtoScreenX( timeline->getInOutRange().min ) - args.x) < 7;
	hoveringOut = abs( normalizedXtoScreenX( timeline->getInOutRange().max ) - args.x) < 7;
}

void ofxTLInOut::mousePressed(ofMouseEventArgs& args){
    
    if(!bounds.inside(args.x,args.y)) return;
       
    
	float distToIn = normalizedXtoScreenX( timeline->getInOutRange().min ) - args.x;
    if(abs(distToIn) < 7){
        draggingIn = true;
        dragOffset = distToIn;
    }
    
    float distToOut = normalizedXtoScreenX( timeline->getInOutRange().max ) - args.x;
    if(!draggingIn && abs(distToOut) < 7){
        draggingOut = true;
        dragOffset = distToOut;
    }

    if(!draggingIn && !draggingOut){
        if(abs(distToOut) > abs(distToIn)){
            draggingIn = true;
            dragOffset = 0;
            timeline->setInPointAtPercent(screenXtoNormalizedX(args.x));            
        }
        else{
            draggingOut = true;
            dragOffset = 0;
            timeline->setOutPointAtPercent(screenXtoNormalizedX(args.x));
        }
    }
    
    //cout << "dist to in " << abs(distToIn) << " out " << abs(distToOut) << " " << draggingIn << " " << draggingOut << endl;
}

void ofxTLInOut::mouseDragged(ofMouseEventArgs& args){
    if(draggingIn){
        timeline->setInPointAtPercent(screenXtoNormalizedX(args.x - dragOffset));
    }
    if(draggingOut){
        timeline->setOutPointAtPercent(screenXtoNormalizedX(args.x - dragOffset));
    }
}

void ofxTLInOut::mouseReleased(ofMouseEventArgs& args){
    if(draggingIn || draggingOut){
	    draggingIn = draggingOut = false;
        save();
    }
}

void ofxTLInOut::load(){
    
	ofxXmlSettings settings;
	if(!settings.loadFile(xmlFileName)){
		ofLog(OF_LOG_NOTICE, "ofxTLInOut -- couldn't load zoom settings file " + xmlFileName);
        timeline->setInOutRange(ofRange(0,1.0));
		return;
	}
	settings.pushTag("inout");
	timeline->setInOutRange(ofRange(settings.getValue("in", 0.0),settings.getValue("out", 1.0)));
	settings.popTag();
    
}

void ofxTLInOut::save(){

	ofxXmlSettings savedSettings;
	savedSettings.addTag("inout");
	savedSettings.pushTag("inout");
	savedSettings.addValue("in", timeline->getInOutRange().min);
	savedSettings.addValue("out", timeline->getInOutRange().max);
	savedSettings.popTag();//zoom
	savedSettings.saveFile(xmlFileName);    
}

