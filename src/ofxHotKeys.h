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

#ifndef OFX_MODIFIER_KEYS
#define OFX_MODIFIER_KEYS

//#include "ofxModifierKeys.h"
#include "ofMain.h"


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
static bool ofGetModifierAltPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_ALT);
}

//----------------------------------------
static bool ofGetModifierShiftPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_SHIFT);
}

//----------------------------------------
static bool ofGetModifierControlPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_CTRL);
}

//----------------------------------------
static bool ofGetModifierSpecialPressed(){
	return ofGetModifierPressed(OF_MODIFIER_KEY_SPECIAL);
}

#endif