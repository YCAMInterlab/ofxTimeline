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

#pragma once

#include "ofMain.h"
#include "ofxTLKeyframes.h"
typedef enum {
    OFXTL_CAMERA_EASE_LINEAR,
    OFXTL_CAMERA_EASE_SMOOTH,
    OFXTL_CAMERA_EASE_CUT
} CameraTrackEase;

//custom keyframe container
//inherits time and value from the super class
class ofxTLCameraFrame : public ofxTLKeyframe {
  public:
	//an example of how to add more information into this keyframe
	//just make some random colors
	ofVec3f position;
	ofQuaternion orientation;
    CameraTrackEase easeIn;
    CameraTrackEase easeOut;
	bool easeInSelected;
};

//Just a simple useless random color keyframer
//to show how to create a custom keyframer
class ofxTLCameraTrack : public ofxTLKeyframes {
  public:
	ofxTLCameraTrack();
	virtual ~ofxTLCameraTrack();

	virtual void enable();
	virtual void disable();
	
	void setTimelineInOutToTrack();
	bool lockCameraToTrack;
	void setCamera(ofCamera& cam);
	
	//a value from 0 - 1 (where 1 freezes the camera, and 0 is no dampening);
	void setDampening(float damp);
	float getDampening();
    
    //call this when you want to stick to the main target ignoring dampening
	void jumpToTarget();
    
	//draw your keyframes
	//some default style is done before this
	virtual void draw();
	virtual void draw3d();
	
	//you can implement custom behavior here, but can get tricky
	//with when to call the super class.
	//see some of the other tracks for interesting patterns on how to
	//extend default interaction behavior
	virtual bool mousePressed(ofMouseEventArgs& args, long millis);
	virtual void mouseMoved(ofMouseEventArgs& args, long millis);
	virtual void mouseDragged(ofMouseEventArgs& args, long millis);
	virtual void mouseReleased(ofMouseEventArgs& args, long millis);
	
	//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
	virtual void keyPressed(ofKeyEventArgs& args);

	//time range contains MIN and MAX time in milliseconds
	//valueRange is 0 at the bottom of the track, and 1 at the top
	//if you have anything other than small dots keyframes you'll want to override
	//the default behavior
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);

	//return a custom name for this keyframe
	virtual string getTrackType();

  protected:
	ofCamera* camera;
	
	static CameraTrackEase getNextEase(CameraTrackEase ease);
    static CameraTrackEase getPreviousEase(CameraTrackEase ease);

	void moveCameraToTime(unsigned long long millis);
	void setCameraFrameToTime(ofxTLCameraFrame* target, unsigned long long millis);
	void interpolateBetween(ofxTLCameraFrame* target,
							ofxTLCameraFrame* prev,
							ofxTLCameraFrame* sample1,
							ofxTLCameraFrame* sample2,
							ofxTLCameraFrame* next, unsigned long long millis);
	float dampening;
	void moveCameraToPosition(ofxTLCameraFrame* target);
	
	void update(ofEventArgs& args);
    
    //convenient drawing functions
    void draweEase(CameraTrackEase ease, ofPoint screenPoint, bool easeIn);
	
	
	//always return the type for your track, in our case ofxTLEmptyKeyframe;
	//this will enusre that all keyframe objects passed to this class are of this type
	virtual ofxTLKeyframe* newKeyframe();
	//load this keyframe out of xml, which is alraedy pushed to the right level
	//only need to save custom properties that our subclass adds
	virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	//save custom properties into the xml
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);

	//return keyframe at this mouse point if you have non circular keyframes
	virtual ofxTLKeyframe* keyframeAtScreenpoint(ofVec2f p);
	
	//responde to right clicks on keyframes
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);
	
	//you can responde to a keyframe about to be killed with this function right before the parent class deletes it
	virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe);
};