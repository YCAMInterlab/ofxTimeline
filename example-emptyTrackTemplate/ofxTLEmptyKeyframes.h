#pragma once

#include "ofMain.h"
#include "ofxTLKeyframes.h"

//custom keyframe container
//inherits time and value from the super class
class ofxTLEmptyKeyframe : public ofxTLKeyframe {
  public:
	//an example of how to add more information into this keyframe
	//just make some random colors
	ofColor color;
};

//Just a simple useless random color keyframer
//to show how to create a custom keyframer
class ofxTLEmptyKeyframes : public ofxTLKeyframes {
  public:
	ofxTLEmptyKeyframes();
	virtual ~ofxTLEmptyKeyframes();

	//draw your keyframes
	//some default style is done before this
	virtual void draw();
	
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

	//how to get colors from our example, linearly interpolated RGB
	ofColor getCurrentColor();
	ofColor getColorAtTime(unsigned long millis);
	
	//time range contains MIN and MAX time in milliseconds
	//valueRange is 0 at the bottom of the track, and 1 at the top
	//if you have anything other than small dots keyframes you'll want to override
	//the default behavior
    virtual void regionSelected(ofLongRange timeRange, ofRange valueRange);

	//return a custom name for this keyframe
	virtual string getTrackType();

  protected:
	//always return the type for your track, in our case ofxTLEmptyKeyframe;
	//this will enusre that all keyframe objects passed to this class are of this type
	virtual ofxTLKeyframe* newKeyframe();
	//load this keyframe out of xml, which is alraedy pushed to the right level
	//only need to save custom properties that our subclass adds
	virtual void restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);
	//save custom properties into the xml
    virtual void storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore);

	//responde to right clicks on keyframes
    virtual void selectedKeySecondaryClick(ofMouseEventArgs& args);
	
	//you can responde to a keyframe about to be killed with this function right before the parent class deletes it
	virtual void willDeleteKeyframe(ofxTLKeyframe* keyframe);
};