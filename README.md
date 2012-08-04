# ofxTimeline #
![Timeline](http://www.jamesgeorge.org/images/ofxtimeline/github/Header.png)

ofxTimeline is a simple to use openFrameworks addon for graphically editing values over time.

It's is a lot like other openFrameworks GUIs: connecting parameters in an app to an editable graphical interface. ofxTimeline has the concept of time built into it, letting you create smoothly changing values or events occurring at specific durations. Borrowing concepts from Flash, Ableton Live, Adobe After Effects and Final Cut Pro to give your app powerful time controls in openFrameworks with just a few lines of code.

## General tools for specific projects ##

ofxTimeline is an inquiry into how reusable components for building intuitive applications can change the way we create. 

Interface based applications for creative media are most commonly expensive big box applications like After Effects or Cinema4d, which deny creative reinterpretation or modification by their users. Open source alternatives are dependent on tied up in huge projects like [Blender](http://blender.org), [VLMC](http://www.videolan.org/vlmc/) and [Audacity](http://audacity.sourceforge.net/). Even very talented programmers have trouble extracting efficient reusable tools from these applications. Moreover their [GPL licensing](http://www.gnu.org/copyleft/gpl.html) is a deal breaking restriction for inclusion in creative coding platforms like openFrameworks, Processing, and Cinder that require commercially viable licenses.

If an ecosystem of components for building small and intuitive media applications were to exist, what new processes could we expose? What collaborations between creative coders and other designers and artists emerge? 

ofxTimeline and is the product of the [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org). 

Licensed under the [MIT License](http://opensource.org/licenses/mit-license.php/) - go crazy, guilt free.

## Major features ##

- Saving and loading of values to XML
- Creation of curves and smooth interpolation
- Zooming in for precise control and long durations
- Smooth playback with precise timing
- Loading of audio and video files with thumbnail and waveform previews
- Convenient controls, such as multi select, copy+paste, arrow key nudging, and snapping
- Paging and view resizing
- Ability to set Beats Per Minute measure marks conjunction with snapping
- Easy to implement your own tracks
- With a growing eco system of community developed track and extensions

## Examples ##

### Simple Tweener ### {example-simple}
Simple Tweener shows how to use two keyframes and a switcher to animate and color a cube

### Audio Waveform ### {example-audio}
Audio Waveform shows how to add an audio track to a project.

### Video Renderer ### {example-video}
Video Renderer shows how to build a simple video effects app using the timeline and a shader. Drag & Drop a video into the application, edit and export the modified frames as an image sequence

### Multi-timeline (experimental) ### {exmaple-multitimeline}
Muli-timeline shows how to use Flags to trigger start and stop the playback of other timelines in the same application.

## Using ofxTimeline in your project ##
ofxTimeline requires a few different add ons

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



## Supported By ##

ofxTimeline was developed as part of the  [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org) at the [YCAMInterLab](http://interlab.ycam.jp/)

![YCAM](http://www.jamesgeorge.org/images/ofxtimeline/github/BangTrack.png)

### Additional support  ###

[Playmodes](http://www.playmodes.com/) who graciously funded and shaped the project in it's early stages for (BlueBeams) 
[The Frank-Ratchye STUDIO for Creative Inquiry](http://studioforcreativeinquiry.org/) who have supported the RGBDToolkit project
[Eyebeam](http://http://eyebeam.org/)

Also a huge thank you to [Creative Applications](creativeapplications.net) for [publishing the initial experiment](http://www.creativeapplications.net/openframeworks/ofxtimeline-openframeworks/) which brought a lot of interested people together to get this to where it is now.

Cover of the timeline sequence from [Blue Beams](https://vimeo.com/35931265)  by [Playmodes](http://www.playmodes.com/)
