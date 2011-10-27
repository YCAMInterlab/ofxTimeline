/*
 *  ofxTLImageSequenceFrame.cpp
 *  RGBDPost
 *
 *  Created by Jim on 10/19/11.
 *  Copyright 2011 University of Washington. All rights reserved.
 *
 */

#include "ofxTLImageSequenceFrame.h"

#include "ofxTLImageSequenceFrame.h"
#include "ofFileUtils.h"

ofxTLImageSequenceFrame::ofxTLImageSequenceFrame()
{
	
	filename = "";
	
	frame = new ofImage();
	frame->setUseTexture(false);
	thumbnail = new ofImage();
	thumbnail->setUseTexture(false);
	
	frameLoaded = false;
	thumbLoaded = false;
	lastUsedTime = ofGetElapsedTimef();
	desiredThumbWidth = 1280/4;
	
	type = OF_IMAGE_UNDEFINED;
}

ofxTLImageSequenceFrame::~ofxTLImageSequenceFrame()
{
	frame->clear();
	thumbnail->clear();
	delete frame;
	delete thumbnail;
}

void ofxTLImageSequenceFrame::setFrame(string _filename)
{
	filename = _filename;
    //to write to
    string path = ofFilePath::getEnclosingDirectory( _filename );
    shortFilename = ofFilePath::getFileName( _filename );
	
    thumbFilename = path + "thumbs/" + shortFilename;
}

ofImage* ofxTLImageSequenceFrame::getFrame()
{
	if(!frameLoaded){
		loadFrame();
	}
	
	lastUsedTime = ofGetElapsedTimef();
	
	//	cout << "last used time " << shortFilename << " is " << lastUsedTime << endl;
	
	return frame;
}

ofImage* ofxTLImageSequenceFrame::getThumbnail()
{
	if(!thumbLoaded){
		loadThumb();
	}
	
	//useCount = 0;
	lastUsedTime = ofGetElapsedTimef();
	//usedThisFrame = true;
	
	return thumbnail;
}

bool ofxTLImageSequenceFrame::isFrameLoaded()
{
	return frameLoaded;
}

bool ofxTLImageSequenceFrame::isThumbLoaded()
{
	return thumbLoaded;
}

void ofxTLImageSequenceFrame::clear()
{
	frame->clear();
	thumbnail->clear();
    frame->setUseTexture(false);
	thumbnail->setUseTexture(false);
	
	frameLoaded = false;
	thumbLoaded = false;
	lastUsedTime = ofGetElapsedTimef();
}

void ofxTLImageSequenceFrame::setType(ofImageType _type)
{
	type = _type;
}

bool ofxTLImageSequenceFrame::loadFrame()
{
    if(frameLoaded){
        return true;
    }
	if(filename == ""){
		ofLog(OF_LOG_ERROR, "ofxTLImageSequenceFrame - ERROR - Loading frame without file name");
		return false;
	}
	
	if(!frame->loadImage(filename)){
		ofLog(OF_LOG_ERROR, "ofxTLImageSequenceFrame - ERROR - failed to load file " + filename);
		return false;
	}
	
    //do this before converting the image type
    if(!thumbLoaded){
        thumbnail->clear();
        thumbnail->setUseTexture(false);
        thumbnail->clone(*frame);
        thumbWidth = desiredThumbWidth;
        float scaleFactor = 1.0*frame->getWidth() / thumbWidth;
        thumbHeight = frame->getHeight() / scaleFactor;
        thumbnail->resize(thumbWidth, thumbHeight);
        thumbnail->saveImage(thumbFilename);
        if(type != OF_IMAGE_UNDEFINED && thumbnail->getPixelsRef().getImageType() != type){
            thumbnail->setImageType(type);
        }
        thumbLoaded = true;
    }
	
	if(type != OF_IMAGE_UNDEFINED && frame->getPixelsRef().getImageType() != type){
		frame->setImageType(type);
	}
	
	frameWidth = frame->getWidth();
	frameHeight = frame->getHeight();
	
	frameLoaded = true;
	return true;
}

void ofxTLImageSequenceFrame::setDesiredThumbnailWidth(int width)
{
	desiredThumbWidth = width;
}


bool ofxTLImageSequenceFrame::loadThumb()
{
    if(thumbLoaded){
        return true;
    }
	
	if(filename == ""){
		ofLog(OF_LOG_ERROR, "ofxTLImageSequenceFrame - ERROR - Loading frame without file name");
		return false;
	}
	
    //first check if there is a thumbnail
	if(!ofFile::doesFileExist(thumbFilename)){
		return loadFrame();
    }
    thumbnail->clear();
    thumbnail->setUseTexture(false);
    if(!thumbnail->loadImage(thumbFilename)){
		ofLog(OF_LOG_ERROR, "ofxTLImageSequenceFrame - ERROR - thumbnail file exists " + thumbFilename + " but was corrputed. reloading." );
        return loadFrame();
    }
	
    if(thumbnail->getWidth() != desiredThumbWidth){
        ofLog(OF_LOG_ERROR, "ofxTLImageSequenceFrame - ERROR - thumbnail file exists " + thumbFilename + " but is the wrong size. reloading." );
        return loadFrame();
    }
	
	if(type != OF_IMAGE_UNDEFINED && thumbnail->getPixelsRef().getImageType() != type){
		thumbnail->setImageType( type );
    }
	
    thumbWidth = thumbnail->getWidth();
    thumbHeight = thumbnail->getHeight();
	
    thumbLoaded = true;
	return true;
}

int ofxTLImageSequenceFrame::getFullFrameWidth() {
	return frameWidth;
}

int ofxTLImageSequenceFrame::getFullFrameHeight() {
    return frameHeight;
}

int ofxTLImageSequenceFrame::getThumbWidth() {
    return thumbWidth;
}

int ofxTLImageSequenceFrame::getThumbHeight() {
    return thumbHeight;
}
