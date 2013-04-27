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

//sourced from here:
// http://stackoverflow.com/questions/6690743/glut-window-menu-bar-in-mac-osx
//
#include "ofxRemoveCocoaMenu.h"
#ifdef TARGET_OSX
#import <Cocoa/Cocoa.h>
#endif

void RemoveCocoaMenusFromGlut(string appName){
#ifdef TARGET_OSX
	if (NSApp){
		NSMenu      *menu;
		NSMenuItem  *menuItem;
		NSAutoreleasePool* p = [[NSAutoreleasePool alloc] init];
//		NSLog(@"%@",[[NSApp mainMenu] description]);
//
		[NSApp setMainMenu:[[NSMenu alloc] init]];
		NSMenu* existingMenu = [NSApp mainMenu];
		
		menu = [[NSMenu alloc] initWithTitle:@""];
		[menu addItemWithTitle:@"About..." action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
		if(appName == ""){
			[menu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
		}
		else {
			[menu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", [NSString stringWithUTF8String:appName.c_str()]]  action:@selector(terminate:) keyEquivalent:@"q"];
		}
		
		menuItem = [[NSMenuItem alloc] initWithTitle:@"Apple" action:nil keyEquivalent:@""];
		[menuItem setSubmenu:menu];
		[[NSApp mainMenu] addItem:menuItem];
		[NSApp performSelector:@selector(setAppleMenu:) withObject:menu];
		
		[p release];
	}
#endif
}

