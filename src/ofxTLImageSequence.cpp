/**
 * ofxTimeline
 *	
 * Copyright (c) 2011 James George
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
 * ----------------------
 *
 * ofxTimeline 
 * Lightweight SDK for creating graphic timeline tools in openFrameworks
 */

#include "ofxTLImageSequence.h"

bool framesort(ofxTLImageSequenceFrame* a, ofxTLImageSequenceFrame* b) {
    return a->lastUsedTime < b->lastUsedTime;
}

ofxTLImageSequence::ofxTLImageSequence() {
	
}

ofxTLImageSequence::~ofxTLImageSequence() {
	
}

void ofxTLImageSequence::setup(){
	maxThumbsLoaded = 300;
	maxFramesLoaded = 50;
	enable();
}


void ofxTLImageSequence::draw() {
	if(!loaded){
		return;
	}
	
    //cout << "preview textures size is " << previewTextures.size() << " " << endl;
	
	for(int i = 0; i < previewTextures.size(); i++){
		ofRectangle b = previewTextures[i].bounds;
		previewTextures[i].texture->draw(bounds.x + b.x, bounds.y + b.y, b.width, b.height);
	}
	
}


bool ofxTLImageSequence::loadSequence(string directory){

    if(loaded){
		clear();
    }
	
    cout << "LOADING SEQUENCE " << directory << endl;
	
    loaded = false;
	
	ofDirectory list;
	list.allowExt("png");
	list.allowExt("jpg");
	
	int numFiles = list.listDir(directory);
	if(numFiles == 0){
		ofLog(OF_LOG_ERROR, "THIS_Sequence -- ERROR -- Loaded sequence with no valid frames " + directory);
		return false;
	}
	
	cout << "FOUND " << numFiles << endl;
	
    //create thumb directory
    if(!ofDirectory::doesDirectoryExist(directory+"/thumbs/")){
        ofDirectory::createDirectory(directory+"/thumbs/");
    }
	
	if(imageType == OF_IMAGE_UNDEFINED){
		//sniff the first file
		ofImage testImage;
		if(!testImage.loadImage(list.getPath(0))){
			ofLogError("ofxTLImageSequence -- error loading test file: " + list.getPath(0));
		}
		imageType = testImage.getPixelsRef().getImageType();
	}
	
	for(int i = 0; i < numFiles; i++){
		string frameFileName =  list.getPath(i);
		ofxTLImageSequenceFrame* frame = new ofxTLImageSequenceFrame();
		frame->setType(imageType);
		frame->setFrame(frameFileName);
		frames.push_back(frame);
	}
	
	cout << "CREATING FRAMES " << numFiles << endl;
	
    frames[0]->loadFrame();
	
	imageWidth = frames[0]->getFullFrameWidth();
    imageHeight = frames[0]->getFullFrameHeight();
	
	thumbWidth = frames[0]->getThumbWidth();
    thumbHeight = frames[0]->getThumbHeight();
	
	loaded = true;
	
    recomputePreview();
	
    cout << "DONE CREATING FRAMES " << numFiles << " img " << imageWidth << "x" << imageHeight << " thumb " << thumbWidth << "x" << thumbHeight << endl;
	
}

float ofxTLImageSequence::getImageWidth(){
	return imageWidth;
}

float ofxTLImageSequence::getImageHeight(){
	return imageHeight;
}

float ofxTLImageSequence::getThumbWidth(){
	return thumbWidth;
}

float ofxTLImageSequence::getThumbHeight(){
	return thumbHeight;
}

ofImage* ofxTLImageSequence::getImageAtTime(float time){
	return getImageAtFrame(time*frames.size()-1);
}

ofImage* ofxTLImageSequence::getImageAtFrame(int frame){
	if(frame >= frames.size()){
		ofLog(OF_LOG_ERROR, "THISSequence -- accessing index %d when we only have %d frames. Returning last frame instead.", frame, frames.size());
		frame = frames.size()-1;
	}
//	if(thumb){
//		return frames[frame]->getThumbnail();
//	}
	return frames[frame]->getFrame();
	
}

void ofxTLImageSequence::drawRectChanged(){
	recomputePreview();
}

void ofxTLImageSequence::setZoomBounds(ofRange zoomBoundsPercent){
	ofxTLElement::setZoomBounds(zoomBoundsPercent);
	recomputePreview();
}

void ofxTLImageSequence::mousePressed(ofMouseEventArgs& args){
	
}

void ofxTLImageSequence::mouseMoved(ofMouseEventArgs& args){
}

void ofxTLImageSequence::mouseDragged(ofMouseEventArgs& args, bool snapped){
}

void ofxTLImageSequence::mouseReleased(ofMouseEventArgs& args){
}

void ofxTLImageSequence::keyPressed(ofKeyEventArgs& args){
}

void ofxTLImageSequence::save(){
}

void ofxTLImageSequence::load(){
}

void ofxTLImageSequence::reset(){
}

void ofxTLImageSequence::clear(){
}

void ofxTLImageSequence::recomputePreview(){
	
	if(!loaded){
		ofLogError("ofxTLImageSequence -- hasn't been loaded");
		return;
	}
	
	if(bounds.height < 20){
		ofLogError("ofxTLImageSequence -- too squished at height " + ofToString(20));
		return;
	}
	
	float widthPerFrame = bounds.height/imageHeight * imageWidth;
	int framesToShow = (bounds.width / widthPerFrame) + 1;
	if(framesToShow == 0){
		ofLogError("ofxTLImageSequence -- no frames to show!");
		return;
	}
	
	
	int startIndex = getIndexAtPercent(zoomBounds.min);
	int endIndex = getIndexAtPercent(zoomBounds.max);
	int framesInRange = (endIndex - startIndex);

	int frameStep = framesInRange / framesToShow;
	int fameIndex = 0;
//	cout << " start index is " << startIndexDec << " end index " << endIndexDec << " frames in range " << framesInRange << " framestep " << frameStep << " frames to show " << framesToShow << endl;
	
	clearPreviewTextures();
	
	for(int i = 0; i < framesToShow; i++){
		ofImage* thumbnail = frames[startIndex+frameStep*i]->getThumbnail();
		PreviewTexture p;
		
		p.frameIndex = startIndex+frameStep*i;
		p.texture = new ofTexture();
		p.texture->allocate(thumbnail->getWidth(), thumbnail->getHeight(), ofGetGlInternalFormat(thumbnail->getPixelsRef()));
		p.texture->loadData(thumbnail->getPixels(), thumbnail->getWidth(), thumbnail->getHeight(), ofGetGlInternalFormat(thumbnail->getPixelsRef()));
		p.bounds = ofRectangle(widthPerFrame*i, 0, widthPerFrame, bounds.height);

//		cout << " preview texture for frame " << startIndex+framesInRange*i << endl;
		
		previewTextures.push_back( p );
	}	
}

int ofxTLImageSequence::getIndexAtPercent(float percent)
{
	return ofMap(percent, 0, 1.0, 0, frames.size()-1, true);
}

float ofxTLImageSequence::getPercentAtIndex(int index){
	return ofMap(index, 0, frames.size()-1, 0, 1, true);
}

void ofxTLImageSequence::purgeFrames()
{
	vector<ofxTLImageSequenceFrame*> sortableFrames;
	for(int i = 0; i < frames.size(); i++){
        if(frames[i]->isFrameLoaded()){
			sortableFrames.push_back( frames[i] );
		}
	}
	
	if(sortableFrames.size() == 0){
		return;
	}
	
	sort(sortableFrames.begin(), sortableFrames.end(), framesort);
	
	cout << " currently " << sortableFrames.size() << " frames loaded. Oldest: " << (ofGetElapsedTimef() - sortableFrames[0]->lastUsedTime) << ". Newest: " << (ofGetElapsedTimef() - sortableFrames[sortableFrames.size()-1]->lastUsedTime) << endl;
	
    if(sortableFrames.size() > maxFramesLoaded){
		
		//cout << "total loaded " << totalLoaded << " use count is " << maxUseCount << " total this frame " << usedThisFrame << " " << thisframefilename <<  endl;
        int numToClear = sortableFrames.size() - maxFramesLoaded;
		
		for(int i = 0; i < numToClear; i++){
			sortableFrames[i]->clear();
		}
    }
}

void ofxTLImageSequence::purgeThumbs()
{
	vector<ofxTLImageSequenceFrame*> sortableFrames;
	for(int i = 0; i < frames.size(); i++){
        if(frames[i]->isThumbLoaded()){
			sortableFrames.push_back( frames[i] );
		}
	}
	
    if(sortableFrames.size() > maxThumbsLoaded){
		
		//cout << "total loaded " << totalLoaded << " use count is " << maxUseCount << " total this frame " << usedThisFrame << " " << thisframefilename <<  endl;
		
		sort(sortableFrames.begin(), sortableFrames.end(), framesort);
		
        int numToClear = sortableFrames.size() - maxThumbsLoaded;
		cout << "purging " << numToClear << " thumbs.  Oldest: " << sortableFrames[0]->lastUsedTime << endl;
		for(int i = 0; i < numToClear; i++){
			sortableFrames[i]->clear();
		}
    }
}

void ofxTLImageSequence::clearPreviewTextures()
{
	for(int i = 0; i < previewTextures.size(); i++){
		delete previewTextures[i].texture;
	}
	previewTextures.clear();
}

void ofxTLImageSequence::clearFrames()
{
	for(int i = 0; i < frames.size(); i++){
		delete frames[i];
	}
	frames.clear();
}
