# ofxTimeline #
![Timeline](http://www.jamesgeorge.org/images/ofxtimeline/github/Header.png)

ofxTimeline is a simple to use openFrameworks add on for creating a user interface to an editable timeline. The framework allows you to compose sequences of change over time, controlling the variables in your openFrameworks application to create time based interactions.

If you're familiar with timelines from programs like After Effects, Final Cut, or Blender then ofxTimeline should feel comfortable. With just a few lines of code you can add a visual editor to smoothly interpolate curves, colors, video, oscillators, audio, and 3d cameras. 

You can also extend ofxTimeline with custom tracks to work with other types of time based data. Check out [Designing Custom Tracks](#designing-custom-tracks)

Copyright 2011-2012 [James George](http://www.jamesgeorge.org)

Co-developed at [YCAM InterLab](http://interlab.ycam.jp/)

Licensed under the [MIT License](http://opensource.org/licenses/mit-license.php/) - go crazy, guilt free.

## Philosophy of Reusable Tools##

ofxTimeline is part of my reserach into how creating GUI based tools within creative coding environments can open up new ways of creating and interacting with media.

Interface based timeline systems are most commonly found in expensive big box applications like After Effects or Cinema4d, which can be prohibitively complex and aren't open source. There are open timeline implementations are tied up in huge projects like [Blender](http://blender.org), [VLMC](http://www.videolan.org/vlmc/) and [Audacity](http://audacity.sourceforge.net/). Even very talented programmers have trouble extracting reusable pieces from these applications for creative reuse. Moreover their [GPL licensing](http://www.gnu.org/copyleft/gpl.html) have commercially restrictive licenses, a deal breaker for inclusion in creative coding platforms like openFrameworks, Processing, and Cinder.

If the community were to build many small components for creating small and intuitive applications what new possibilities would we uncover? What collaborations between creative coders and other designers and artists would emerge? 

## Major features ##
- Very few lines of code to include in openFrameworks project
- Precise millisecond level timing and drift-free playback
- Several track types built in, such as curved, colors, and video tracks with thumbnail previews
- Convenient controls for efficient editing, such as multi-select, copy+paste, arrow keys nudging, and snapping to points
- Zooming in for detailed control and editing timelines with long durations
- Saving and loading of values to XML
- Adding multiple pages
- Ability to set beats per minute measure marks conjunction with snapping
- Easily extended for implementing your own tracks


## Getting started

Here are a series of video tutorials for getting started:

https://vimeo.com/52302437

https://vimeo.com/52304312

https://vimeo.com/52304313

password: ycam


## Anatomy of ofxTimeline
![Anatomy](http://www.jamesgeorge.org/images/ofxtimeline/github/AnatomyOfTimeline.jpg)

### 1 Pages 

You can divide the timeline into pages, each page containing its own set of tracks.

### 2 Ticker and Playhead

The header displays tick marks at minutes, seconds, and milliseconds. Clicking inside the header allows you to scrube the playhead time. The green line extending down the height of the timeline indicates the current time, while the red line  displays the hovering time.

### 3 In/Out Handles 

The bar below the ticker allows you to drag the in and out points on the timeline. The region defined by the in and out point is all that is included during playback.

### 4 Tracks 

The main body of the timeline is a list of tracks that are editable and resizable.

### 5 Zoomer

The control at the bottom of the track allows for zooming in and precisely editing time.

## Examples ##

### Simple Curves
Simple Curves shows how to use two keyframes and a switcher to animate and color a cube
![SimpleTween](http://www.jamesgeorge.org/images/ofxtimeline/github/example-simpleTween.png)

### Audio Waveform
Audio Waveform shows how to add an audio track to a project.
![AudioWaveform](http://www.jamesgeorge.org/images/ofxtimeline/github/example-audioWaveform.png)

### Video Renderer
Video Renderer shows how to build a simple video effects app using the timeline and a shader. Drag & Drop a video into the application, edit and export the modified frames as an image sequence
![VideoRenderer](http://www.jamesgeorge.org/images/ofxtimeline/github/example-videoRenderer.png)

### Multi-timeline
Muli-timeline shows how to use Flags to trigger start and stop the playback of other timelines in the same application.
![MultiTimeline](http://www.jamesgeorge.org/images/ofxtimeline/github/example-multiTimeline.png)

## Hotkeys

ofxTimeline has a few hot keys built in

### Enabling Hotkeys on OS X
On OS X, GLUT overrides COMMAND+C, COMMAND+V, and COMMAND+S behavior. ofxTimeline has a function to remove these default bindings in order that the key commands in the timeline work. Add the following line of code to your setup function:

      ofxTimeline::removeCocoaMenusFromGlut("myAppName");

### All Hotkeys

Note on OS X the COMMAND key is used, on Linux and Windows the CTRL key is used

<table>
    <tr>
        <td style="font-weight:bold">Function</td><td style="font-weight:bold">Shortcut</td>
    </tr>
    <tr>
        <td>Cut Selection</td><td>command+x</td>
    </tr>
    <tr>
        <td>Copy Selection</td><td>command+c</td>
    </tr>
    <tr>
        <td>Paste Selection</td><td>command+v</td>
    </tr>
    <tr>
        <td>Undo</td><td>command+z</td>
    </tr>
    <tr>
        <td>Redo</td><td>shift+command+z</td>
    </tr>
    <tr>
        <td>Select all keyframes in Focused track</td><td>command+a</td>
    </tr>
    <tr>
        <td>Add all keyframes in Focused track to selection</td><td>command+shift+a</td>
    </tr>
    <tr>
        <td>Delete all selected keyframes</td><td>delete or backspace</td>
    </tr>    
    <tr>
        <td>Nudge keyframes a little</td><td>arrow keys</td>
    </tr>    
    <tr>
        <td>Nudge keyframes a little more</td><td>shift+arrow keys</td>
    </tr>    
    <tr>
        <td>Expand Focused track</td><td>alt+e</td>
    </tr>    
    <tr>
        <td>Collapse all tracks</td><td>alt+c</td>
    </tr>    
    <tr>
        <td>Evenly distribute track sizes</td><td>alt+shift+c</td>
    </tr>    
</table>




## Including ofxTimeline in your openFrameworks project

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
![Curves](http://www.jamesgeorge.org/images/ofxtimeline/github/CurvesTrack.png)

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


### ColorTrack
![ColorTrack](http://www.jamesgeorge.org/images/ofxtimeline/github/ColorTrack.png)

The color track let's you sequence smoothly changing colors. It uses a provided image as a sample palette, each keyframe specifies a position on the palette to sample from. Between keyframes the image is traverse to create smoothly changing colors.

### LFO (experimental)
![LFO](http://www.jamesgeorge.org/images/ofxtimeline/github/LFOTrack.png)

The LFO, Low Frequency Oscillator, track defines simple repeating sine waves and noise functions. 

## 3rd Party Custom Tracks ##

ofxTimeline can be integrated with other addons or time based media, below are examples of just a few

### DepthImageSequence ###

https://github.com/obviousjim/ofxRGBDepth // [RGBDToolkit](http://wwww.rgbdtoolkit.com)

### MidiTrack ###

https://github.com/momo-the-monster/ofxTimelineTracks // [ofxTLMidiNote](https://vimeo.com/46793363)

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
