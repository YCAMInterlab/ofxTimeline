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

//----------------------------------------
static bool ofGetModifierKeyAlt(){
	return (glutGetModifiers() & GLUT_ACTIVE_ALT);
}

//----------------------------------------
static bool ofGetModifierKeyShift(){	
	return (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
}

//----------------------------------------
static bool ofGetModifierKeyControl(){
	return (glutGetModifiers() & GLUT_ACTIVE_CTRL);	
}

