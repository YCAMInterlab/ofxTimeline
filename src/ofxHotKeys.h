/*
 *  ofxTLUtils.h
 *  timebasedTimelineDemo
 *
 *  Created by Jim on 12/22/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 *  This is here until it gets included in the core openFrameworks
 *  taken from https://github.com/openframeworks/openFrameworks/pull/807/commits
 */

#pragma once

//#include "ofxModifierKeys.h"
#include "ofMain.h"

/*
#ifdef TARGET_WIN32
	#define GLUT_BUILDING_LIB
	 #include "glut.h"
#endif

#ifdef TARGET_OSX
  #include "../../../libs/glut/lib/osx/GLUT.framework/Versions/A/Headers/glut.h"
  #include <Carbon/Carbon.h>
#endif

#ifdef TARGET_LINUX
  #include <GL/glut.h>
#endif
*/

enum ofxModifierKey {
	OF_MODIFIER_KEY_SHIFT = 1,
	OF_MODIFIER_KEY_CTRL = 2,
 	OF_MODIFIER_KEY_ALT  = 4,
	OF_MODIFIER_KEY_SPECIAL = 8
};

//platform specific
bool ofGetModifierPressed(ofxModifierKey key);

//convention for elements to be selected
//COMMAND or SHIFT for OSX, just CTRL on windows
bool ofGetModifierSelection();


//COMMAND on OS X, CTRL on Windows
//----------------------------------------
bool ofGetModifierShortcutKeyPressed();

//----------------------------------------
bool ofGetModifierAltPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_ALT);
}

//----------------------------------------
bool ofGetModifierShiftPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_SHIFT);
}

//----------------------------------------
bool ofGetModifierControlPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_CTRL);
}

//----------------------------------------
bool ofGetModifierSpecialPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_SPECIAL);
}



class ofxHotKeys {
  public:
	
};