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

#include "ofxTLColorTrack.h"
#include "ofxTimeline.h"

ofxTLColorTrack::ofxTLColorTrack()
 :	drawingColorWindow(false),
	clickedInColorRect(false),
	defaultColor(ofColor(0,0,0)),
	previousSample(NULL),
	nextSample(NULL),
	setNextAndPreviousOnUpdate(false)

{
	//
}

void ofxTLColorTrack::draw(){
	
	if(bounds.height == 0){
		return;
	}
	
	if(viewIsDirty || shouldRecomputePreviews){
		updatePreviewPalette();
	}
	
	if(keyframes.size() == 0){
		ofPushStyle();
		ofSetColor(defaultColor);
		ofFill();
		ofRect(bounds);
		ofPopStyle();
	}
	else if(keyframes.size() == 1){
		ofPushStyle();
		ofxTLColorSample* s = (ofxTLColorSample*)keyframes[0];
		ofSetColor(s->color);
		ofFill();
		ofRect(bounds);
		ofPopStyle();
	}
	else{
		previewPalette.draw(bounds);
	}
	
	for(int i = 0; i < keyframes.size(); i++){
		
		if(!isKeyframeIsInBounds(keyframes[i])){
			continue;
		}
		
		float screenX = millisToScreenX(keyframes[i]->time);
		
		ofPoint a = ofPoint(screenX-10,bounds.y);
		ofPoint b = ofPoint(screenX+10,bounds.y);
		ofPoint c = ofPoint(screenX,bounds.y+10);
		
		ofPushStyle();
		ofFill();
		ofxTLColorSample* s = (ofxTLColorSample*)keyframes[i];
		ofSetColor(s->color);
		ofTriangle(a,b,c);
		ofNoFill();
		ofSetColor(s->color.getInverted());
		ofSetLineWidth(1);
		ofTriangle(a,b,c);

		if(keyframes[i] == hoverKeyframe){
			ofSetColor(timeline->getColors().highlightColor);
			ofSetLineWidth(3);
		}
		else if(isKeyframeSelected(keyframes[i])){
			ofSetColor(timeline->getColors().textColor);
			ofSetLineWidth(2);
		}
		else{
			ofSetColor(s->color.getInverted());
		}
		ofLine(c, ofVec2f(screenX, bounds.getMaxY()));
		ofPopStyle();
	}
}

void ofxTLColorTrack::drawModalContent(){
	if(drawingColorWindow){
		
		//this happens when a new keyframe is added
		//we need to wait until the draw cycle for the new
		//key to be in the array so we can determine it's
		//surrounding samples
		if(setNextAndPreviousOnUpdate){
			setNextAndPreviousSamples();
			setNextAndPreviousOnUpdate = false;
		}
		
		if(selectedKeyframe == NULL){
			ofLogError("ofxTLColorTrack::drawModalContent") << "The selected keyframe is null" << endl;
			drawingColorWindow = false;
			timeline->dismissedModalContent();
			return;
		}
		
		if(!colorPallete.bAllocated()){
			ofLogError("ofxTLColorTrack::drawModalContent") << "The color palette is not allocated" << endl;
			timeline->dismissedModalContent();
			drawingColorWindow = false;
		}
		ofPushStyle();
		ofFill();
		ofSetColor(255);
		
		ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
		colorWindow = ofRectangle( millisToScreenX(selectedKeyframe->time), bounds.y+bounds.height, 200, 200);
		if(colorWindow.getMaxY()+25 > timeline->getBottomLeft().y){
			colorWindow.y = bounds.y - 25 - colorWindow.height;
		}
		if(colorWindow.getMaxX() > ofGetWidth()){
			colorWindow.x -= colorWindow.width;
		}
		colorPallete.draw(colorWindow);
		
		ofVec2f selectionPoint = colorWindow.getMin() + selectedSample->samplePoint * ofVec2f(colorWindow.width,colorWindow.height);
		ofSetColor(selectedSample->color.getInverted());
		ofLine(selectionPoint - ofVec2f(8,0), selectionPoint + ofVec2f(8,0));
		ofLine(selectionPoint - ofVec2f(0,8), selectionPoint + ofVec2f(0,8));
		
		ofPushStyle();
		ofNoFill();
		if(previousSample != NULL){
			ofVec2f previousSamplePoint = colorWindow.getMin() + previousSample->samplePoint * ofVec2f(colorWindow.width,colorWindow.height);
			ofSetColor(previousSample->color.getInverted(), 150);
			ofCircle(previousSamplePoint, 3);
			ofLine(previousSamplePoint,selectionPoint);
		}
		if(nextSample != NULL){
			ofVec2f nextSamplePoint = colorWindow.getMin() + nextSample->samplePoint * ofVec2f(colorWindow.width,colorWindow.height);
			ofSetColor(nextSample->color.getInverted(), 150);

			//draw a little triangle pointer
			ofVec2f direction = (nextSamplePoint - selectionPoint).normalized();
			ofVec2f backStep = nextSamplePoint-direction*5;
			ofTriangle(nextSamplePoint, 
					   backStep + direction.getRotated(90)*3,
					   backStep - direction.getRotated(90)*3);
			ofLine(nextSamplePoint,selectionPoint);
		}
		ofPopStyle();
		
		previousColorRect = ofRectangle(colorWindow.x, colorWindow.getMaxY(), colorWindow.width/2, 25);
		newColorRect = ofRectangle(colorWindow.x+colorWindow.width/2, colorWindow.getMaxY(), colorWindow.width/2, 25);

		ofSetColor(colorAtClickTime);
		ofRect(previousColorRect);
		ofSetColor(selectedSample->color);
		ofRect(newColorRect);
		ofSetColor(timeline->getColors().keyColor);
		ofNoFill();
		ofSetLineWidth(2);
		ofRect(colorWindow);
		ofPopStyle();
	}	
}

void ofxTLColorTrack::loadColorPalette(ofBaseHasPixels& image){
	colorPallete.setFromPixels(image.getPixelsRef());
	refreshAllSamples();
}

bool ofxTLColorTrack::loadColorPalette(string imagePath){
	if(colorPallete.loadImage(imagePath)){
		palettePath = imagePath;
		refreshAllSamples();
		return true;
	}
	return false;
}

ofColor ofxTLColorTrack::getColor(){
	//play solo change
//	return getColorAtMillis(timeline->getCurrentTimeMillis());
	return getColorAtMillis(currentTrackTime());
}

ofColor ofxTLColorTrack::getColorAtSecond(float second){
	return getColorAtMillis(second*1000);
}

ofColor ofxTLColorTrack::getColorAtPosition(float pos){
	return getColorAtMillis(pos * timeline->getDurationInMilliseconds());
}

ofColor ofxTLColorTrack::getColorAtMillis(unsigned long long millis){
	if(keyframes.size() == 0){
		return defaultColor;
	}
	
	if(millis <= keyframes[0]->time){
		//cout << "getting color before first key " << ((ofxTLColorSample*)keyframes[0])->color << endl;
		return ((ofxTLColorSample*)keyframes[0])->color;
	}
	
	if(millis >= keyframes[keyframes.size()-1]->time){
		return ((ofxTLColorSample*)keyframes[keyframes.size()-1])->color;
	}
	
	for(int i = 1; i < keyframes.size(); i++){
		if(keyframes[i]->time >= millis){
			ofxTLColorSample* startSample = (ofxTLColorSample*)keyframes[i-1];
			ofxTLColorSample* endSample = (ofxTLColorSample*)keyframes[i];
			float interpolationPosition = ofMap(millis, startSample->time, endSample->time, 0.0, 1.0);
			return samplePaletteAtPosition(startSample->samplePoint.getInterpolated(endSample->samplePoint, interpolationPosition));
		}
	}
	ofLogError("ofxTLColorTrack::getColorAtMillis") << "Could not find color for millis " << millis << endl;
	return defaultColor;
}

void ofxTLColorTrack::setDefaultColor(ofColor color){
	defaultColor = color;
}

ofColor ofxTLColorTrack::getDefaultColor(){
	return defaultColor;	
}

string ofxTLColorTrack::getPalettePath(){	
	if(palettePath == ""){
		ofLogWarning("ofxTLColorTrack::getPalettePath -- Palette is not loaded from path");
	}
	return palettePath;
}

//For selecting keyframe type only,
//the superclass controls keyframe placement
bool ofxTLColorTrack::mousePressed(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		clickedInColorRect = args.button == 0 && colorWindow.inside(args.x, args.y);
		if(clickedInColorRect){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			selectedSample->samplePoint = ofVec2f(ofMap(args.x, colorWindow.getX(), colorWindow.getMaxX(), 0, 1.0-FLT_EPSILON, true),
												  ofMap(args.y, colorWindow.getY(), colorWindow.getMaxY(), 0, 1.0-FLT_EPSILON, true));
			refreshSample(selectedSample);
			shouldRecomputePreviews = true;
		}
		else if(args.button == 0 && previousColorRect.inside(args.x, args.y)){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			selectedSample->samplePoint = samplePositionAtClickTime;
			refreshSample(selectedSample);
			clickedInColorRect = true; //keep the window open
			shouldRecomputePreviews = true;			
		}
		
		return true;
	}
	else{
		return ofxTLKeyframes::mousePressed(args, millis);
	}
}

void ofxTLColorTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		if(clickedInColorRect){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			selectedSample->samplePoint = ofVec2f(ofMap(args.x, colorWindow.getX(), colorWindow.getMaxX(), 0, 1.0-FLT_EPSILON,true),
												  ofMap(args.y, colorWindow.getY(), colorWindow.getMaxY(), 0, 1.0-FLT_EPSILON,true));
			refreshSample(selectedSample);
			shouldRecomputePreviews = true;
		}
	}
	else{
		ofxTLKeyframes::mouseDragged(args, millis);
		if(keysDidDrag){
			shouldRecomputePreviews = true;
		}
	}
}

void ofxTLColorTrack::mouseReleased(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		//if(args.button == 0 && !colorWindow.inside(args.x, args.y) ){
		if(args.button == 0 && !clickedInColorRect){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			if(selectedSample->color != colorAtClickTime){
				timeline->flagTrackModified(this);
			}			
			timeline->dismissedModalContent();
			drawingColorWindow = false;
			shouldRecomputePreviews = true;
		}
	}
	else{
		ofxTLKeyframes::mouseReleased(args, millis);
	}
}

void ofxTLColorTrack::keyPressed(ofKeyEventArgs& args){
	if(drawingColorWindow){
		if(args.key == OF_KEY_RETURN){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			if(selectedSample->color != colorAtClickTime){
				timeline->flagTrackModified(this);
			}
			timeline->dismissedModalContent();
			drawingColorWindow = false;
			shouldRecomputePreviews = true;
		}
	}
	else{
		ofxTLKeyframes::keyPressed(args);
	}
}

void ofxTLColorTrack::updatePreviewPalette(){
	
	if(previewPalette.getWidth() != bounds.width){
		previewPalette.allocate(bounds.width, 1, OF_IMAGE_COLOR); //someday support alpha would be rad
	}
	
	if(keyframes.size() == 0 || keyframes.size() == 1){
		return; //we just draw solid colors in this case
	}

	previewPalette.setUseTexture(false);
	for(int i = 0; i < bounds.width; i++){
		previewPalette.setColor(i, 0, getColorAtMillis(screenXToMillis(bounds.x+i)));
	}
	previewPalette.setUseTexture(true);
	previewPalette.update();

	shouldRecomputePreviews = false;
}

ofxTLKeyframe* ofxTLColorTrack::newKeyframe(){
	ofxTLColorSample* sample = new ofxTLColorSample();
	sample->samplePoint = ofVec2f(.5,.5);
	sample->color = defaultColor;
	//when creating a new keyframe select it and draw a color window
	colorAtClickTime = defaultColor;
	samplePositionAtClickTime = sample->samplePoint;
	drawingColorWindow = true;
	clickedInColorRect = true;
	//find surrounding points next drawModal cycle
	setNextAndPreviousOnUpdate = true;
	timeline->presentedModalContent(this);
	return sample;
}

void ofxTLColorTrack::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLColorSample* sample = (ofxTLColorSample*)key;
	
	sample->samplePoint = ofVec2f(xmlStore.getValue("sampleX", 0.0),
								  xmlStore.getValue("sampleY", 0.0));
	
	//for pasted keyframes cancel the color window
	drawingColorWindow = false;
	timeline->dismissedModalContent();
	refreshSample(sample);
}

void ofxTLColorTrack::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLColorSample* sample = (ofxTLColorSample*)key;
	xmlStore.setValue("sampleX", sample->samplePoint.x);
	xmlStore.setValue("sampleY", sample->samplePoint.y);
}

void ofxTLColorTrack::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
    	if(timeRange.contains( keyframes[i]->time )){
            selectKeyframe(keyframes[i]);
        }
	}
}

void ofxTLColorTrack::selectedKeySecondaryClick(ofMouseEventArgs& args){

	drawingColorWindow = true;
	previousSample = NULL;
	nextSample = NULL;

	colorAtClickTime = ((ofxTLColorSample*)selectedKeyframe)->color;
	samplePositionAtClickTime = ((ofxTLColorSample*)selectedKeyframe)->samplePoint;
	
	timeline->presentedModalContent(this);
	setNextAndPreviousSamples();
}

void ofxTLColorTrack::setNextAndPreviousSamples(){
	previousSample = nextSample = NULL;
	for(int i = 0; i < keyframes.size(); i++){
		if(keyframes[i] == selectedKeyframe){
			if(i > 0){
				previousSample = (ofxTLColorSample*)keyframes[i-1];
			}
			if(i < keyframes.size()-1){
				nextSample = (ofxTLColorSample*)keyframes[i+1];
			}
			break;
		}
	}
}


ofxTLKeyframe* ofxTLColorTrack::keyframeAtScreenpoint(ofVec2f p){
	if(isHovering()){
		for(int i = 0; i < keyframes.size(); i++){
			float offset = p.x - timeline->millisToScreenX(keyframes[i]->time);
			if (abs(offset) < 5) {
				return keyframes[i];
			}
		}
	}
	return NULL;
}

void ofxTLColorTrack::refreshAllSamples(){
	for(int i = 0; i < keyframes.size(); i++){
		refreshSample((ofxTLColorSample*)keyframes[i]);
	}
	shouldRecomputePreviews = true;
}

void ofxTLColorTrack::refreshSample(ofxTLColorSample* sample){
	sample->color = samplePaletteAtPosition(sample->samplePoint);
}

//assumes normalized position
ofColor ofxTLColorTrack::samplePaletteAtPosition(ofVec2f position){
	if(colorPallete.bAllocated()){
		ofVec2f positionPixelSpace = position * ofVec2f(colorPallete.getWidth(),colorPallete.getHeight());
		//bilinear interpolation from http://www.gamedev.net/page/resources/_/technical/graphics-programming-and-theory/bilinear-interpolation-of-texture-maps-r810
		int x0 = int(positionPixelSpace.x);
		int y0 = int(positionPixelSpace.y);
		float dx = positionPixelSpace.x-x0, dy = positionPixelSpace.y-y0, omdx = 1-dx, omdy = 1-dy;
		return colorPallete.getPixelsRef().getColor(x0,y0)*omdx*omdy +
	           colorPallete.getPixelsRef().getColor(x0,MIN(y0+1, colorPallete.getHeight()-1))*omdx*dy +
               colorPallete.getPixelsRef().getColor(MIN(x0+1,colorPallete.getWidth()-1),y0)*dx*omdy +
			   colorPallete.getPixelsRef().getColor(MIN(x0+1,colorPallete.getWidth()-1),MIN(y0+1, colorPallete.getHeight()-1))*dx*dy;
	}
	else{
		ofLogError("ofxTLColorTrack::refreshSample -- sampling palette is null");
		return defaultColor;
	}
}

string ofxTLColorTrack::getTrackType(){
	return "Colors";
}
