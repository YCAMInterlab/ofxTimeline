# ofxTimeline controls values over time. #


## using ofxTimeline in your project ##
ofxTimeline requires a few different add ons

# Track Types #

ofxTimeline has several tracks

## Bangs ##

Inheritance  hierarchy:
ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs

## Flags ##

Inheritance  hierarchy:
ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs -> ofxTLFlags

## Keyframes ##
ofxTLTrack -> ofxTLKeyframes 

## Tweens ##
ofxTLTrack -> ofxTLKeyframes -> ofxTLTweens

## Switches ##
ofxTLTrack -> ofxTLBangs -> ofxTLSwitches

## VideoTrack ##

ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack
ofxTLVideoTrack can control time

## AudioTrack ##
![AudioTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/AudioTrack.png)

ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack
ofxTLVideoTrack can control time

External Tracks
## CameraTrack ##
ofxTLTrack -> ofxTLBangs -> ofxTLCameraTrack

## DepthImageSequence

## MidiTrack ##


# Designing custom Tracks 

The track system is designed to be extensible. Based on the type 

A few rules to keep in mind


# Dependencies

You'll need the following addons for compiling ofxTimeline:

ofxXmlSettings (core library)

ofxRange ( https://github.com/Flightphase/ofxRange )

ofxTween ( https://github.com/arturoc/ofxTween )

ofxTextInputField ( https://github.com/Flightphase/ofxTextInputField )

