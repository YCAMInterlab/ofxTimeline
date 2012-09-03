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
	shouldRecalculatePreview(false),
	clickedInColorRect(false),
	defaultColor(ofColor(0,0,0))
{
	//
}

void ofxTLColorTrack::draw(){
	
	if(viewIsDirty || shouldRecalculatePreview){
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
		ofSetColor(timeline->getColors().keyColor);
		ofTriangle(a,b,c);
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
			
		colorWindow = ofRectangle( millisToScreenX(selectedKeyframe->time), bounds.y, 150, 150);
		colorPallete.draw(colorWindow);
	}	
}

void ofxTLColorTrack::loadColorPalette(ofBaseHasPixels& image){
	colorPallete.setFromPixels(image.getPixelsRef());
}

void ofxTLColorTrack::loadColorPalette(string imagePath){
	if(colorPallete.loadImage(imagePath)){
		palettePath = imagePath;
	}
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
		clickedInColorRect = colorWindow.inside(args.x, args.y);
		return true;
	}
	else{
		return ofxTLKeyframes::mousePressed(args, millis);
	}
}

void ofxTLColorTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		if(clickedInColorRect){
			
		}
	}
	else{
		ofxTLKeyframes::mouseDragged(args, millis);
		if(keysDidDrag){
			shouldRecalculatePreview = true;
		}
	}
}

void ofxTLColorTrack::mouseReleased(ofMouseEventArgs& args, long millis){
	if(drawingColorWindow){
		if(colorWindow.inside(args.x, args.y)){
			ofxTLColorSample* selectedSample = (ofxTLColorSample*)selectedKeyframe;
			selectedSample->samplePoint = ofVec2f(ofMap(args.x, colorWindow.getX(), colorWindow.getMaxX(), 0, 1.0),
												  ofMap(args.y, colorWindow.getY(), colorWindow.getMaxY(), 0, 1.0));
			refreshSample(selectedSample);
			shouldRecalculatePreview = true;
		}
		cout << "dissmissing modal content" << endl;
		timeline->dismissedModalContent();
		drawingColorWindow = false;
		shouldRecalculatePreview = true;
	}
	else{
		ofxTLKeyframes::mouseReleased(args, millis);
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
	
	/*
	ofColor startColor = getColorAtMillis(screenXToMillis(0));
	colorMesh.addVertex(bounds.getMin());
	colorMesh.addColor(startColor);
	colorMesh.addVertex(ofVec2f(bounds.getMinX(),bounds.getMaxY()));
	colorMesh.addColor(startColor);
	for(int i = 0; i < keyframes.size(); i++){
		ofxTLColorSample* sample = (ofxTLColorSample*)keyframes[i];
		int screenX = millisToScreenX(sample->time);
		colorMesh.addVertex(ofVec2f(screenX, bounds.getMinY()));
		colorMesh.addColor(sample->color);
		colorMesh.addVertex(ofVec2f(screenX, bounds.getMaxY()));
	}
	
	ofColor endColor = getColorAtMillis(screenXToMillis(bounds.getMaxX()));
	colorMesh.addVertex(ofVec2f(bounds.getMaxX(),bounds.getMinY()));
	colorMesh.addColor(endColor);
	colorMesh.addVertex(bounds.getMax());
	colorMesh.addColor(endColor);
	
	//index it!
	for(int i = 4; i <= colorMesh.getVertices().size(); i+=2){
		cout << "adding two triangles" << endl;
		//top left
		colorMesh.addIndex(i-4);
		colorMesh.addIndex(i-2);
		colorMesh.addIndex(i-3);

		colorMesh.addIndex(i-2);
		colorMesh.addIndex(i-1);
		colorMesh.addIndex(i-3);
	}
	*/
	
	shouldRecalculatePreview = false;
}

ofxTLKeyframe* ofxTLColorTrack::newKeyframe(){
	ofxTLColorSample* sample = new ofxTLColorSample();
	sample->samplePoint = ofVec2f(0,0);
	sample->color = defaultColor;
	//when creating a new keyframe select it and draw a color window
	drawingColorWindow = true;
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
	//TODO show sample window
}

ofxTLKeyframe* ofxTLColorTrack::keyframeAtScreenpoint(ofVec2f p){
    if(isActive()){
        for(int i = 0; i < keyframes.size(); i++){
            float offset = p.x - timeline->millisToScreenX(keyframes[i]->time);
            if (abs(offset) < 5) {
                return keyframes[i];
            }
        }
    }
	return NULL;
}

void ofxTLColorTrack::refreshSample(ofxTLColorSample* sample){
	sample->color = samplePaletteAtPosition(sample->samplePoint);
}

//assumes normalized position
ofColor ofxTLColorTrack::samplePaletteAtPosition(ofVec2f position){
	if(colorPallete.bAllocated()){
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
