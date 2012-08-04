# WORK IN PROGRESS - DO NOT PUBLICIZE #
This project is part of a three month research residency at YCAM. I'm prototyping the code and philosophy in public, inviting many of you who have found your way here to share and contribute ideas. I hope that after the residency I can publish a developed theory and practice together. For the moment however it's important that this project stay under the radar as the ideas are still in formation.

Thanks! 

# ofxTimeline #
![Timeline](http://www.jamesgeorge.org/images/ofxtimeline/github/Header.png)
Cover of the timeline sequence from [Blue Beams](https://vimeo.com/35931265)  by [Playmodes](http://www.playmodes.com/)

ofxTimeline is a simple to use openFrameworks addon for graphically editing values over time.

It's a lot like other openFrameworks GUIs that connect your parameters in code to an editable graphical interface. However ofxTimeline has the concept of time built into it, letting you create smoothly changing values or events occurring at specific moments. Borrowing concepts from Flash, Ableton Live, Adobe After Effects and Final Cut Pro to give your app powerful time controls with just a few lines of code.

## General tools for specific projects ##

ofxTimeline is an inquiry into how reusable components for building intuitive applications can change the way we create. 

Interface based applications for creative media are most commonly expensive big box applications like After Effects or Cinema4d, which deny creative reinterpretation or modification by their users. Open source alternatives are tied up in huge projects like [Blender](http://blender.org), [VLMC](http://www.videolan.org/vlmc/) and [Audacity](http://audacity.sourceforge.net/). Even very talented programmers have trouble extracting reusable pieces from these applications for creative reimagination. Moreover their [GPL licensing](http://www.gnu.org/copyleft/gpl.html) is a deal breaking restriction for inclusion in creative coding platforms like openFrameworks, Processing, and Cinder that require commercially viable licenses.

If an ecosystem of components for building small and intuitive media applications were to exist, what new processes could we invent? What collaborations between creative coders and other designers and artists would emerge? 

ofxTimeline is the product of the [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org). 

Licensed under the [MIT License](http://opensource.org/licenses/mit-license.php/) - go crazy, guilt free.

## Major features ##
- Very few lines of code to use
- Precise millisecond level timing, nano-second accurate no drift playback
- Several usable track types out of the box, such as curved tweens and video tracks with thumbnail previews
- Convenient controls for efficient editing, such as multi-select, copy+paste, arrow keys nudging, and snapping to points
- Zooming in for detailed control and long durations
- Loading of audio and video files with thumbnail and waveform previews
- Saving and loading of values to XML
- Adding multiple pages and dragging to resize view
- Ability to set Beats Per Minute measure marks conjunction with snapping
- Easy to implement your own tracks
- A growing eco system of community developed track and extensions


## Examples ##

### Simple Tweener
Simple Tweener shows how to use two keyframes and a switcher to animate and color a cube

### Audio Waveform
Audio Waveform shows how to add an audio track to a project.

### Video Renderer
Video Renderer shows how to build a simple video effects app using the timeline and a shader. Drag & Drop a video into the application, edit and export the modified frames as an image sequence

### Multi-timeline (experimental)
Muli-timeline shows how to use Flags to trigger start and stop the playback of other timelines in the same application.


## Controls ##


### Editing ###

### Zooming ###

### Selecting elements ###

### Resizing ###


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

    class MyClass {
    
      ofxTimeline timeline;
      void receivedBang(ofxTLBangEventArgs& bang);
      
    };

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

### Tweens ###
![Tweens](http://www.jamesgeorge.org/images/ofxtimeline/github/TweenTrack.png)

Tweens change a value between a min and max range smoothly over time, edited with keyframes that have interpolation.

Right clicking a keyframe on the timeline brings up a selection window to change the interpolation value. These are based on the Penner equations found in [ofxTween](https://github.com/arturoc/ofxTween)

    Inheritance: ofxTLTrack -> ofxTLKeyframes -> ofxTLTweens

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

To add an AudioTrack 

    ofxTimeline timeline;
    ofxTLAudioWaveform waveform;

In your .cpp file

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
    		//timeline.togglePlay();
    		waveform.togglePlay();
    	}

The timeline's duration must match the audio's duration.

    ofxTLTrack -> ofxTLImageTrack -> ofxTLVideoTrack


External Custom Tracks
### CameraTrack ###

ofxTLTrack -> ofxTLBangs -> ofxTLCameraTrack

### DepthImageSequence ###

### MidiTrack ###


## Designing custom Tracks ##

The track system is designed to be extensible. 



## Supported By ##

ofxTimeline was developed as part of the  [Guest Research Projector v.2 at YCAM Interlab](http://interlab.ycam.jp/en/projects/guestresearch/vol2) by [James George](http://www.jamesgeorge.org) at the [YCAMInterLab](http://interlab.ycam.jp/)

![YCAM](http://www.jamesgeorge.org/images/ofxtimeline/github/logo_ycam.png)

### Additional support  ###

![Additional Support](http://www.jamesgeorge.org/images/ofxtimeline/github/SupportedBy.png)

[Playmodes](http://www.playmodes.com/) who graciously funded and shaped the project in it's early stages for [BlueBeams](https://vimeo.com/35931265) .

[The Frank-Ratchye STUDIO for Creative Inquiry](http://studioforcreativeinquiry.org/) who have supported the [RGBDToolkit](http://www.rgbdtoolkit.com) project.

[Eyebeam](http://http://eyebeam.org/) Who's community has prompted many of the thoughts provoking the development of ofxTimeline during my residency in Spring 2012.

Also a huge thank you to [Creative Applications](creativeapplications.net) for [publishing the initial experiment](http://www.creativeapplications.net/openframeworks/ofxtimeline-openframeworks/) which brought a lot of interested people together to get this to where it is now.
