//
//  ofxTLColorTrack.cpp
//  Duration
//
//  Created by Jim on 8/31/12.
//
//

#include "ofxTLColorTrack.h"
#include "ofxTimeline.h"

ofxTLColorTrack::ofxTLColorTrack()
 :	drawingColorWindow(false),
	clickedInColorRect(false),
	defaultColor(ofColor(0,0,0)),
	previousSample(NULL),
	nextSample(NULL)

{
	//
}

void ofxTLColorTrack::draw(){
	
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
		
		ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
		colorWindow = ofRectangle( millisToScreenX(selectedKeyframe->time), bounds.y+bounds.height, 200, 200);
		colorPallete.draw(colorWindow);
		
		ofVec2f selectionPoint = colorWindow.getMin() + selectedSample->samplePoint * ofVec2f(colorWindow.width,colorWindow.height);
		ofSetColor(selectedSample->color.getInverted());
		ofLine(selectionPoint - ofVec2f(8,0), selectionPoint + ofVec2f(8,0));
		ofLine(selectionPoint - ofVec2f(0,8), selectionPoint + ofVec2f(0,8));
		
		ofSetColor(colorAtClickTime);
		ofRect(colorWindow.x, colorWindow.getMaxY(), colorWindow.width/2, 25);
		ofSetColor(selectedSample->color);
		ofRect(colorWindow.x+colorWindow.width/2, colorWindow.getMaxY(), colorWindow.width/2, 25);
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
	return getColorAtMillis(timeline->getCurrentTimeMillis());
}

ofColor ofxTLColorTrack::getColorAtSecond(float second){
	return getColorAtMillis(second*1000);
}

ofColor ofxTLColorTrack::getColorAtPosition(float pos){
	return getColorAtMillis(pos * timeline->getDurationInMilliseconds());
}

ofColor ofxTLColorTrack::getColorAtMillis(unsigned long millis){
	if(keyframes.size() == 0){
		return defaultColor;
	}
	
	if(millis < keyframes[0]->time){
		return ((ofxTLColorSample*)keyframes[0])->color;
	}
	
	if(millis > keyframes[keyframes.size()-1]->time){
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
			selectedSample->samplePoint = ofVec2f(ofMap(args.x, colorWindow.getX(), colorWindow.getMaxX(), 0, 1.0),
												  ofMap(args.y, colorWindow.getY(), colorWindow.getMaxY(), 0, 1.0));
			refreshSample(selectedSample);
			shouldRecomputePreviews = true;
		}
		
		return true;
	}
	else{
		return ofxTLBangs::mousePressed(args, millis);
	}
}

void ofxTLColorTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		if(clickedInColorRect){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			selectedSample->samplePoint = ofVec2f(ofMap(args.x, colorWindow.getX(), colorWindow.getMaxX(), 0, 1.0),
												  ofMap(args.y, colorWindow.getY(), colorWindow.getMaxY(), 0, 1.0));
			refreshSample(selectedSample);
			shouldRecomputePreviews = true;
		}
	}
	else{
		ofxTLBangs::mouseDragged(args, millis);
		if(keysDidDrag){
			shouldRecomputePreviews = true;
		}
	}
}

void ofxTLColorTrack::mouseReleased(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		if(args.button == 0 && !colorWindow.inside(args.x, args.y) ){
			timeline->dismissedModalContent();
			drawingColorWindow = false;
			shouldRecomputePreviews = true;
		}
	}
	else{
		ofxTLBangs::mouseReleased(args, millis);
	}
}

void ofxTLColorTrack::keyPressed(ofKeyEventArgs& args){
	if(drawingColorWindow){
		if(args.key == OF_KEY_RETURN){
			timeline->dismissedModalContent();
			drawingColorWindow = false;			
		}
	}
	else{
		ofxTLBangs::keyPressed(args);
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
	sample->samplePoint = ofVec2f(0,0);
	sample->color = defaultColor;
	//when creating a new keyframe select it and draw a color window
	colorAtClickTime = defaultColor;
	drawingColorWindow = true;
	clickedInColorRect = true;
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

void ofxTLColorTrack::selectedKeySecondaryClick(ofMouseEventArgs& args){
	if(selectedKeyframe != NULL){
		drawingColorWindow = true;
		colorAtClickTime = ((ofxTLColorSample*)selectedKeyframe)->color;
		timeline->presentedModalContent(this);
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
		//TODO: make this interpolated based on float values to eliminate striping
		return colorPallete.getColor(position.x*colorPallete.getWidth(),position.y*colorPallete.getHeight());
	}
	else{
		ofLogError("ofxTLColorTrack::refreshSample -- sampling palette is null");
		return defaultColor;
	}
}

string ofxTLColorTrack::getTrackType(){
	return "Colors";
}
