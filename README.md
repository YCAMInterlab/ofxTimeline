ofxTimeline is an extensible and lightweight sdk for making graphical timeline tools in openFrameworks.  The project intends to support the sensibility of traditional timeline based animation like in after effects but bring it into a real time environment.  

# Layout

An instance of ofxTimeline contains any number of elements that can be edited along the duration.  It also has organizational elements such as support for multiple pages, time ticker, and view zooming.

http://vimeo.com/33135665

# Built in Elements:

* Keyframer

The keyframer element allows for editing of float values changing over time using specific points in time (keyframes) and interpolation rules to transition from one keyframe to the next

	
* Switcher

The switcher element supports simple boolean on/off editing.

* Trigger

The trigger elements is used for setting individual markers in time that trigger an event with an arbitrary text string. Triggers are useful for initiating generative and code based processes but from a specific point time in relationship to other elements. 


* Image Sequence

The image sequence creates a scrubbable filmstrip of a given sequence of images. It's intended for timelines for controlling video effects on a series of images, and is useful for rapidly prototyping generative video effects

* Video Sequence

The video sequence acts much like the image sequence, but takes a video source file as input. The video sequence also has the ability to control playback of the timeline along with the natural playback of the video, to 


# Extending

ofxTimeline is built to be extensible by adding custom elements. Any sublcass of ofxTLElement can be added to a timeline, and will receive mouse and keyboard events, be given a bounding rectangle.

# Dependencies

You'll need the following addons for compiling ofxTimeline:

ofxXmlSettings (core library)

ofxRange ( https://github.com/Flightphase/ofxRange )

ofxTween ( https://github.com/arturoc/ofxTween )

ofxTextInputField ( https://github.com/Flightphase/ofxTextInputField )

