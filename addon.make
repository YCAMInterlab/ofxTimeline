meta:
	ADDON_NAME = ofxTimeline
	ADDON_DESCRIPTION = ofxTimeline is a simple to use openFrameworks add on for creating a user interface to an editable timeline.
	ADDON_AUTHOR = James George
	ADDON_TAGS = time timeline curves keyframes
	ADDON_URL = http://github.com/obviousjim/ofxTimeline

common:
	ADDON_DEPENDENCIES = ofxXmlSettings
	ADDON_DEPENDENCIES += ofxTween
	ADDON_DEPENDENCIES += ofxRange
	ADDON_DEPENDENCIES += ofxMSATimer
	ADDON_DEPENDENCIES += ofxTimecode
	ADDON_DEPENDENCIES += ofxTextInputField
	
	ADDON_INCLUDES =
	
	ADDON_CFLAGS =
	
	ADDON_LDFLAGS =
	
	ADDON_PKG_CONFIG_LIBRARIES =
	
	ADDON_FRAMEWORKS =
	
	ADDON_SOURCES =
	
	ADDON_LIBS =
	
	ADDON_DATA = copy_to_data_GUI/*
	
linux64:
linux:
osx:
win_cb:
vs2010:
iphone:
android:
linuxarmv6l:
linuxarmv7l:
