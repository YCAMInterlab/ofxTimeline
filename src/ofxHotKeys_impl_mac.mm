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

#include "ofxHotKeys.h"

#ifdef TARGET_OSX

#include <Cocoa/Cocoa.h>

bool ofGetModifierSelection(){
	return ofGetModifierShiftPressed() || ofGetModifierSpecialPressed();
}

bool ofGetModifierShortcutKeyPressed(){
#ifdef MAC_USE_CONTROL
//		cout << "using command" << endl;
	return ofGetModifierControlPressed();
#else
//		cout << "using control" << endl;
	return ofGetModifierSpecialPressed();
#endif
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