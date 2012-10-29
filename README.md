# ofxTimeline #
![Timeline](http://www.jamesgeorge.org/images/ofxtimeline/github/Header.png)
Timleine sequence from [Blue Beams](https://vimeo.com/35931265)  by [Playmodes](http://www.playmodes.com/)

ofxTimeline is a simple to use openFrameworks add on for creating an editable timeline user interface.

If you're familiar with timelines from programs like After Effects, Final Cut, or Blender then ofxTimeline should feel familiar to you. With just a few lines of code you can add a visual editor to smoothly interpolate curves, colors, video, oscillators, audio, and 3d cameras. 

You can also extend ofxTimeline with custom tracks to work with other types of time based data. Check out [Designing Custom Tracks](#designing-custom-tracks)

## General tools for specific projects ##

ofxTimeline is functional tool, but also part of a larger inquiry into how bringing in tools gui based tools in creative coding applications can improve the way we work with the medium.

Interface based applications for creative media are most commonly expensive big box applications like After Effects or Cinema4d, which can be prohibitvely complex and aren't open source. Open source alternatives are tied up in huge projects like [Blender](http://blender.org), [VLMC](http://www.videolan.org/vlmc/) and [Audacity](http://audacity.sourceforge.net/). Even very talented programmers have trouble extracting reusable pieces from these applications for creative reimagination. Moreover their [GPL licensing](http://www.gnu.org/copyleft/gpl.html) is a deal breaking restriction for inclusion in creative coding platforms like openFrameworks, Processing, and Cinder that require commercially viable licenses.

If a lot of small components for building small and intuitive media applications were to exist along with ofxTimeline, what new processes could we invent? What collaborations between creative coders and other designers and artists would emerge? 

ofxTimeline is the product of the [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org). 

Licensed under the [MIT License](http://opensource.org/licenses/mit-license.php/) - go crazy, guilt free.

## Major features ##
- Very few lines of code to use
- Precise millisecond level timing, no drift playback
- Several track types built in, such as curved tweens and video tracks with thumbnail previews
- Convenient controls for efficient editing, such as multi-select, copy+paste, arrow keys nudging, and snapping to points
- Zooming in for detailed control and long durations
- Loading of audio and video files with thumbnail and waveform previews
- Saving and loading of values to XML
- Adding multiple pages and dragging to resize view
- Ability to set Beats Per Minute measure marks conjunction with snapping
- Extensible to implement your own tracks
- A collection of community developed tracks and extensions

## Getting started

Here are a series of video tutorials for getting started

https://vimeo.com/52302437

https://vimeo.com/52304312

https://vimeo.com/52304313

password: ycam

## Examples ##

### Simple Tweener
Simple Tweener shows how to use two keyframes and a switcher to animate and color a cube
![SimpleTween](http://www.jamesgeorge.org/images/ofxtimeline/github/example-simpleTween.png)

### Audio Waveform
Audio Waveform shows how to add an audio track to a project.
![AudioWaveform](http://www.jamesgeorge.org/images/ofxtimeline/github/example-audioWaveform.png)

### Video Renderer
Video Renderer shows how to build a simple video effects app using the timeline and a shader. Drag & Drop a video into the application, edit and export the modified frames as an image sequence
![VideoRenderer](http://www.jamesgeorge.org/images/ofxtimeline/github/example-videoRenderer.png)

### Multi-timeline (experimental)
Muli-timeline shows how to use Flags to trigger start and stop the playback of other timelines in the same application.
![MultiTimeline](http://www.jamesgeorge.org/images/ofxtimeline/github/example-multiTimeline.png)


## Anatomy of ofxTimeline
![Anatomy](http://www.jamesgeorge.org/images/ofxtimeline/github/AnatomyOfTimeline.jpg)

1 Pages 

You can divide the timeline into pages, each page containing it's own set of tracks

2 Ticker and Playhead

The header displays tick marks at seconds, and milliseconds. Clicking the header allows you to change the playhead time. The green line crossing the timeline shows the current playhead time. The red line shows display where you are hovering

3 In and Out Handles. 

The bar below the ticker allows you to drag the in and out points on the timeline. The region defined by the in and out point is all that is played.

4 Tracks 

The main body of the timeline is a list of tracks that are editable and resizable

5 Zoomer

The control at the bottom of the track allows for zooming in and precisely editing time


### Keyframe Selection



## Including ofxTimeline in your openFrameworks project
Follow the video tutorial to see how to setup a new ofxTimeline project. A textual description is below.

### Clone ofxTimeline and dependencies 
to download the timeline open up the terminal and type

    $cd of_0071_osx_release/addons
    $git clone https://github.com/YCAMInterlab/ofxTimeline.git
    $cd ofxTimeline/
    $./clone_addons.sh

This will download the necessary addons, but won't overwrite any changes if you already have some of them installed

### Add ofxTimeline to your project
In Xcode open up the project you'd like to include the timeline. Drag and drop the ofxTimeline folder in from the finder into Xcode navigator under addons/

If you plan on using the Audiowaveform see additional instructions. Otherwise you can remove the examples-*/, and libs/ folder, as well as removeing the referenc to the ofxTLAudioTrack source files

### Add a timeline to your code

in your testApp.h file add:

    #include "ofMain.h"
    #include "ofxTimeline.h" //add timeline include

    class testApp : public ofBaseApp{
    public:
      ofxTimeline timeline; //inside of the class add a timeline

in your setup fon testApp.cpp file set up the timeline

    //--------------------------------------------------------------
    void testApp::setup(){
      
      timeline.setup(); //registers events
      timeline.setDurationInSeconds(60); //sets time
      timeline.setLoopType(OF_LOOP_NORMAL); //turns the timeline to loop
      
      //add a track
      timeline.addKeyframes("MyCircleRadius", ofRange(0, 200));
  
in your draw or update function, read the value
  
    //--------------------------------------------------------------
    void testApp::draw(){
      //the value of changingRadius will be different depending on the timeline
      float changingRadius = timeline.getKeyframeValue("MyCircleRadius"),
      //use the value for something amazing!
      ofCircle(mouseX, mouseY, changingRadius);
      //don't forget to draw your timeline so you can edit it.
      timeline.draw();
    }
    
It's a good convention to put in hotkeys for play and show hide in keyPressed

    //--------------------------------------------------------------
    void testApp::keyPressed(int key){
      if(key == ' '){
        timeline.togglePlay();
      }
      if(key == 'h'){
        timeline.toggleShow();
      }
    }

## Track Types
ofxTimeline has several built in track types for doing standard timeline tasks. 

### Bangs
![Bangs](http://www.jamesgeorge.org/images/ofxtimeline/github/BangTrack.png)

A bang is a simple time marker that sends an event when the playhead passes it. To use a bang track you need a class that listens to bang events, like so:

MyClass.h

      ofxTimeline timeline;
      void receivedBang(ofxTLBangEventArgs& bang);
      
MyClass.cpp

    //--------------------------------------------------------------
    void MyClass::setup(){
      timeline.addBangs("trigger particles");
      ofAddListener(timeline.events().bangFired, this, &testApp::receivedBang);
    }
    
    //--------------------------------------------------------------
    void MyClass::receivedBang(ofxTLBangEventArgs& bang){
      ofLogNotice("Bang fired from track " + bang->track.getName());
      if(bang.track->getName() == "trigger particles"){
        particleGenerator.start(); //some example response
      }
    }
    
.

    Inheritance: ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs

### Curves ###
![Curves](http://www.jamesgeorge.org/images/ofxtimeline/github/TweenTrack.png)

Curves change a value between a min and max range smoothly over time, edited with keyframes that have interpolation.

Right clicking a keyframe on the timeline brings up a selection window to change the interpolation value. These are based on the Penner equations found in [ofxCurves](https://github.com/arturoc/ofxTween)

    Inheritance: ofxTLTrack -> ofxTLKeyframes -> ofxTLCurves

### Flags ###
![Flags](http://www.jamesgeorge.org/images/ofxtimeline/github/FlagTrack.png)

Flags are like Bangs but allow the user to enter text on each flag. The value can be read in the event

    //--------------------------------------------------------------
    void MyClass::receivedBang(ofxTLBangEventArgs& bang){      
      ofLogNotice("Bang fired from track " + bang.flag);
    }   

.

    Inheritance: ofxTLTrack -> ofxTLKeyframes -> ofxTLBangs -> ofxTLFlags

### Switches ###
![Switches](http://www.jamesgeorge.org/images/ofxtimeline/github/SwitchTrack.png)

Switches provide a simple control to turn regions of the timeline on and off.

    Inheritance: ofxTLTrack -> ofxTLBangs -> ofxTLSwitches

### VideoTrack ###
![VideoTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/VideoTrack.png)

VideoTracks let a user interactively scrub through a video and sequence effects in time with the playback. When a video track is added the video playback will control the playback of the entire timeline. 

        ofxTLVideoTrack* videoTrack = timeline.addVideoTrack("Video", videoPath);
        if(videoTrack != NULL){ //in the case the video load failed check against null
            timeline.setFrameRate(videoTrack->getPlayer()->getTotalNumFrames()/videoTrack->getPlayer()->getDuration());
            timeline.setDurationInFrames(videoTrack->getPlayer()->getTotalNumFrames());
            timeline.setTimecontrolTrack(videoTrack); //video playback will control the time        
        }

The timeline's duration must match the video's duration.

    ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack


### AudioTrack ###
![AudioTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/AudioTrack.png)

AudioTracks let a user interactively scrub through an audio track and sequence effects in time.

To add an AudioTrack to your project, add the declaration to your .h file

    ofxTimeline timeline;
    ofxTLAudioWaveform waveform;

And n your .cpp file add the track and load a file

    //--------------------------------------------------------------
    void testApp::keyPressed(int key){
        //... setup stuff
        timeline.addTrack("Track", &waveform);
        waveform.loadSoundfile("myAudioFile.wav");
        timeline.setDurationInSeconds(waveform.getDuration());
    }

    //--------------------------------------------------------------
    void testApp::keyPressed(int key){
        if(key == ' '){
            //calling play on the waveform controls the timeline playback
    		waveform.togglePlay();
    	}
    }

The timeline's duration must match the audio's duration.

    ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack


## 3rd Party Custom Tracks ##

ofxTimeline can be integrated with other addons or time based media, below are examples of just a few

### DepthImageSequence ###

https://github.com/obviousjim/ofxRGBDepth // [RGBDToolkit](http://wwww.rgbdtoolkit.com)

### MidiTrack ###

https://github.com/momo-the-monster/ofxTimelineTracks // [ofxTLMidiNot](https://vimeo.com/46793363)

## Designing custom Tracks ##

The track system is designed to be extensible. There are two empty track templates that are the best place to get started building your own track

### ofxTLEmptyTrack

Use this template as a starting point for visualizing time based data in ofxTimeline or creating a unique type of interactive track.

### ofxTLEmptyKeyframes
Use this template for creating a custom keyframe based track. It already has copy+paste, multiple selection, saving and loading.

## Supported By ##

ofxTimeline is codeveloped by YCAM InterLab during the [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org) at the [YCAMInterLab](http://interlab.ycam.jp/)

![YCAM](http://www.jamesgeorge.org/images/ofxtimeline/github/logo_ycam.png)

### Additional support  ###

![Additional Support](http://www.jamesgeorge.org/images/ofxtimeline/github/SupportedBy.png)

[Playmodes](http://www.playmodes.com/) who graciously funded and shaped the project in it's early stages for [BlueBeams](https://vimeo.com/35931265) .

[The Frank-Ratchye STUDIO for Creative Inquiry](http://studioforcreativeinquiry.org/) who have supported the [RGBDToolkit](http://www.rgbdtoolkit.com) project.

[Eyebeam](http://http://eyebeam.org/) Who's community has prompted many of the thoughts provoking the development of ofxTimeline during my residency in Spring 2012.

Also a huge thank you to [Creative Applications](creativeapplications.net) for [publishing the initial experiment](http://www.creativeapplications.net/openframeworks/ofxtimeline-openframeworks/) which brought a lot of interested people together to get this to where it is now.
