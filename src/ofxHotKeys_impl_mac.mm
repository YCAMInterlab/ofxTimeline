
#include "ofxHotKeys.h"

#ifdef TARGET_OSX

#include <Cocoa/Cocoa.h>

bool ofGetModifierSelection(){
	return ofGetModifierShiftPressed() || ofGetModifierSpecialPressed();
}

bool ofGetModifierPressed(ofxModifierKey mod) {
	unsigned int t = 0;

	if ((OF_MODIFIER_KEY_CTRL & mod) == OF_MODIFIER_KEY_CTRL)
		t += NSControlKeyMask;
	
	if ((OF_MODIFIER_KEY_ALT & mod) == OF_MODIFIER_KEY_ALT)
		t += NSAlternateKeyMask;
	
	if ((OF_MODIFIER_KEY_SHIFT & mod) == OF_MODIFIER_KEY_SHIFT)
		t += NSShiftKeyMask;

	if ((OF_MODIFIER_KEY_SPECIAL & mod) == OF_MODIFIER_KEY_SPECIAL)
		t += NSCommandKeyMask;

	return [[NSApp currentEvent] modifierFlags] & t;
}

#endif