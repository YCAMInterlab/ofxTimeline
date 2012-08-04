# ofxTimeline #
ofxTimeline is a simple to use and extendable openFrameworks addon for graphically editing values over time.

It's is a lot like other openFrameworks GUIs, letting you connect parameters in your app a graphical interface you can edit with a mouse, saving and loading those values to xml. ofxTimeline has the concept of time build into it, letting you create values that change over time or events that happen at fixed durations. It borrows concepts from Flash's timeline, music visualization applications like Ableton Live, and video editing software like Adobe After Effects and Final Cut Pro. All available in openFrameworks with just a few lines of code

Some selling points of ofxTimeline are:

- Saving and loading of values to XML
- Curves and smooth values
- Smooth playback with precise timing
- Loading of audio and video files with thumbnail and waveform previews
- Convenient controls, such as multi select, copy+paste, arrow key nudging, and snapping
- Paging and view resizing
- Ability to set Beats Per Minute measure marks conjunction with snapping
- Easy to implement your own tracks
- With a growing eco system of community developed track and extensions

## Using ofxTimeline in your project ##
ofxTimeline requires a few different add ons


## Examples ##

### Simple Tweener ###

### Audio Waveform ###

### Video Renderer ###

### Multi timeline (experimental) ###

## Track Types ##

ofxTimeline has several built in track types for doing standard timeline tasks. 

### Bangs ###
![Bangs](http://www.jamesgeorge.org/images/ofxtimeline/github/BangTrack.png)
A bang is a simple time marker that sends an event when the playhead passes it.

ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs

### Tweens ###
![Tweens](http://www.jamesgeorge.org/images/ofxtimeline/github/TweenTrack.png)


ofxTLTrack -> ofxTLKeyframes -> ofxTLTweens

### Flags ###
![Flags](http://www.jamesgeorge.org/images/ofxtimeline/github/FlagTrack.png)

Inheritance  hierarchy:
ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs -> ofxTLFlags

### Switches ###
![Switches](http://www.jamesgeorge.org/images/ofxtimeline/github/SwitchTrack.png)

ofxTLTrack -> ofxTLBangs -> ofxTLSwitches

### VideoTrack ###
![VideoTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/VideoTrack.png)

ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack
ofxTLVideoTrack can control time

### AudioTrack ###
![AudioTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/AudioTrack.png)

ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack
AudioTrack can control time

External Custom Tracks
### CameraTrack ###

ofxTLTrack -> ofxTLBangs -> ofxTLCameraTrack

### DepthImageSequence ###

### MidiTrack ###


## Designing custom Tracks ##

The track system is designed to be extensible. 




