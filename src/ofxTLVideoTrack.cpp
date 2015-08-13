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

#include "ofxTLVideoTrack.h"
#include "ofxTimeline.h"

ofxTLVideoTrack::ofxTLVideoTrack() 
	: ofxTLImageTrack()
{
	currentLoop = 0;
	inFrame = -1;
	outFrame = -1;
    currentlyPlaying = false;
    drawVideoPreview = true;
	playAlongToTimeline = true;
	isSetup = false;
}

ofxTLVideoTrack::~ofxTLVideoTrack(){
	if(isSetup){
		disable();
		ofRemoveListener(ofEvents().exit, this, &ofxTLVideoTrack::exit);
	}
}

void ofxTLVideoTrack::setup(){
    ofxTLImageTrack::setup();
	isSetup = true;
    ofAddListener(ofEvents().exit, this, &ofxTLVideoTrack::exit);
    startThread();
}

void ofxTLVideoTrack::enable(){
	if(!enabled){
		ofxTLImageTrack::enable();
//		ofAddListener(ofEvents().update, this, &ofxTLVideoTrack::update);
		ofAddListener(events().playheadScrubbed, this, &ofxTLVideoTrack::playheadScrubbed);
	}
}

void ofxTLVideoTrack::disable(){
	if(enabled){
		stop();
		ofxTLImageTrack::disable();
//		ofRemoveListener(ofEvents().update, this, &ofxTLVideoTrack::update);
		ofRemoveListener(events().playheadScrubbed, this, &ofxTLVideoTrack::playheadScrubbed);
	} 
}

bool ofxTLVideoTrack::togglePlay(){
	
    if(!isLoaded()) return false;
    
    if(getIsPlaying()){
        stop();
    }
    else{
        play();
    }
	return getIsPlaying();
}

void ofxTLVideoTrack::play(){
    if(isLoaded() && !getIsPlaying()){
        if(player->getIsMovieDone()){
			player->setFrame(inFrame);
		}
		if(getCurrentFrame() >= timeline->getOutFrame()){
			player->setFrame(timeline->getInFrame());
		}
//		#ifdef OF_VIDEO_PLAYER_QUICKTIME
		player->setSpeed(1.0);
//		#endif
		player->play();
		ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
		ofNotifyEvent(events().playbackStarted, args);
		
        currentlyPlaying = true;
    }
}

void ofxTLVideoTrack::stop(){
	if(!isLoaded()){
		return;
	}

	player->setSpeed(0);
    if(isLoaded() && getIsPlaying()){
//		cout << "stopping playback" << endl;
		#ifdef OF_VIDEO_PLAYER_QUICKTIME
        player->setSpeed(0.0);
		#else
		player->stop();
		#endif
        ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
        ofNotifyEvent(events().playbackEnded, args);
        currentlyPlaying = false;
//		cout << "player is playing? " << player->isPlaying() << endl;
		if(timeline->getTimecontrolTrack() == this){
			timeline->setPercentComplete(player->getPosition());
		}
    }
}

bool ofxTLVideoTrack::getIsPlaying(){
	//return isLoaded() && player->isPlaying() && player->getSpeed() > 0.0;
	return isLoaded() && currentlyPlaying;
}


void ofxTLVideoTrack::setPlayAlongToTimeline(bool playAlong){
	this->playAlongToTimeline = playAlong;
}

bool ofxTLVideoTrack::getPlayAlongToTimeline(){
	return playAlongToTimeline;
}

//void ofxTLVideoTrack::update(ofEventArgs& args){
void ofxTLVideoTrack::update(){
    
	if(!isLoaded()){
		return;
	}
	
   	if(timeline->getTimecontrolTrack() == this){
		
		if(timeline->getIsFrameBased()){
			timeline->setCurrentFrame(player->getCurrentFrame());
		}
		else {
			timeline->setCurrentTimeSeconds(player->getPosition()*player->getDuration());
		}

   		if(getIsPlaying()){
			//no longer used
			if(player->getCurrentFrame() < inFrame || player->getCurrentFrame() > outFrame){
	//            cout << "reset in frame from " << player->getCurrentFrame() << endl;
				player->setFrame(inFrame);
	//            cout << "	to: " << player->getCurrentFrame() << endl;
			}
			
			if(selectedFrame > player->getCurrentFrame()){
				currentLoop++;
				//cout << "LOOPED! with last frame " << lastFrame << " " << player->getCurrentFrame() << " current loop " << currentLoop << endl;
			}

			//if(timeline->getInOutRange().max < player->getPosition()){
			if(timeline->getOutFrame() <= player->getCurrentFrame() || player->getIsMovieDone()){
	//			cout << " OVER " << endl;
				if(timeline->getLoopType() == OF_LOOP_NONE){
	//				cout << " no loop g" << endl;
					stop();                
				}
				else {
					int loopFrame = timeline->getInFrame();
					selectFrame(loopFrame);
					if(playAlongToTimeline){
						player->play();
					}
					ofxTLPlaybackEventArgs args = timeline->createPlaybackEvent();
					ofNotifyEvent(events().playbackLooped, args);                
				}
			}
			//else if(timeline->getInOutRange().min > player->getPosition()){
			if(timeline->getInFrame() > player->getCurrentFrame()){
				//cout << "looping frame " << timeline->getInFrame()  << endl;
				int loopFrame = timeline->getInFrame();
				selectFrame(loopFrame);
			}
			//timeline->setPercentComplete(player->getPosition());
		}
		else{
//	        if(player->getSpeed() != 0){
//		        player->setSpeed(0);
//	        }
			if(player->isPlaying()){
				stop();
			}
		}
	}
	else {
		if(timeline->getCurrentTime() > player->getDuration()){
			player->stop();
		}
	}
	
	player->update();
//	cout << "timeline frame vs video frame " << player->getCurrentFrame() << " vs " << timeline->getCurrentFrame() << endl;
	selectedFrame = player->getCurrentFrame();
}

void ofxTLVideoTrack::playheadScrubbed(ofxTLPlaybackEventArgs& args){
	
    if(isLoaded() && !currentlyPlaying && playAlongToTimeline){

        selectFrame(args.currentFrame);
//        cout << "after scrub timeline time is " << timeline->getCurrentTime()  << " frame is " << timeline->getCurrentFrame() << " and percent is " << timeline->getPercentComplete() << endl;
//        cout << "while video is " << player->getPosition()*player->getDuration() << " frame is " << player->getCurrentFrame() << " and percent is " << player->getPosition() << endl;
    }
}

float ofxTLVideoTrack::positionForSecond(float second){
	if(isLoaded()){
		return ofMap(second, 0, player->getDuration(), 0, 1.0, true);
	}
	return 0;
}

void ofxTLVideoTrack::playbackStarted(ofxTLPlaybackEventArgs& args){
	ofxTLTrack::playbackStarted(args);
	if(isLoaded() && this != timeline->getTimecontrolTrack() && playAlongToTimeline){
		//player.setPosition(timeline->getPercentComplete());
		float position = positionForSecond(timeline->getCurrentTime());
		if(position < 1.0){
			player->setSpeed(1.0);
			player->play();
            currentlyPlaying = true;
		}
		player->setPosition( position );
	}
}

void ofxTLVideoTrack::playbackLooped(ofxTLPlaybackEventArgs& args){
	if(isLoaded() && this != timeline->getTimecontrolTrack() && playAlongToTimeline){
		if(!player->isPlaying()){
			player->play();
		}
		player->setPosition( positionForSecond(timeline->getCurrentTime()) );
	}
}

void ofxTLVideoTrack::playbackEnded(ofxTLPlaybackEventArgs& args){
	if(isLoaded() && this != timeline->getTimecontrolTrack() && playAlongToTimeline){
		player->stop();
	}
}

void ofxTLVideoTrack::framePositionsUpdated(vector<ofxTLVideoThumb>& newThumbs) {

    for(int i = 0; i < newThumbs.size(); i++){
		newThumbs[i].useTexture = false;        
    }

    //6) copy old textures thumbs over if they are still used, delete them if they are not
    for(int i = 0; i < videoThumbs.size(); i++){
        for(int j = 0; j < newThumbs.size(); j++){
            if(videoThumbs[i].framenum == newThumbs[j].framenum && videoThumbs[i].loaded){
                newThumbs[j].thumb = videoThumbs[i].thumb;
                newThumbs[j].loaded = true;
                break;
            }
        }
    }
    
	backLock.lock();
    backThumbs = newThumbs;
	backLock.unlock();
    
	lock();
    videoThumbs = newThumbs;
    unlock();
}

void ofxTLVideoTrack::threadedFunction(){
    
	while(isThreadRunning()){
        backLock.lock();
        if(!ofGetMousePressed() && !currentlyZooming && thumbsEnabled && backthreadedPlayer != NULL && backthreadedPlayer->isLoaded()){
            for(int i = 0; i < backThumbs.size(); i++){
                if(!backThumbs[i].loaded){
                    if(currentlyZooming || ofGetMousePressed()){
                        break;
                    }
					if(backThumbs[i].framenum < backthreadedPlayer->getTotalNumFrames()){
	                    backthreadedPlayer->setFrame(backThumbs[i].framenum);

						if(currentlyZooming || ofGetMousePressed()){
							break;
						}
						
						backThumbs[i].useTexture = false;
						backThumbs[i].create(backthreadedPlayer->getPixelsRef());
					}
                    
                    lock();
                    videoThumbs[i] = backThumbs[i];
                    unlock();

                }
            }
        }
        backLock.unlock();
     
        ofSleepMillis(100);
    }
}

bool ofxTLVideoTrack::load(string moviePath){
    
    ofPtr<ofVideoPlayer> newPlayer = ofPtr<ofVideoPlayer>(new ofVideoPlayer());    
    if(newPlayer->load(moviePath)){
        setPlayer( newPlayer );
        return true;
    }
    else {
        ofLogError("ofxTLVideoTrack::loadMovie -- movie load failed: " + moviePath);
    }
	return false;
}

void ofxTLVideoTrack::setPlayer(ofVideoPlayer& newPlayer){
	setPlayer(ofPtr<ofVideoPlayer>( &newPlayer ));    
}

void ofxTLVideoTrack::setPlayer(ofPtr<ofVideoPlayer> newPlayer){
    player = newPlayer;
    if(player->isLoaded()){
        
        backLock.lock();
		videoThumbs.clear();
        backthreadedPlayer = ofPtr<ofVideoPlayer>(new ofVideoPlayer());
        backthreadedPlayer->setUseTexture(false);
        backthreadedPlayer->load(player->getMoviePath());
        backLock.unlock();
        
		calculateFramePositions();
		
		inFrame = 0;
		outFrame = player->getTotalNumFrames();
        currentlyPlaying = false;
		player->setLoopState(OF_LOOP_NONE);
        player->setFrame(0);
        player->update();
    }
    else {
		ofLogError("ofxTLVideoTrack::setPlayer -- setting a video player before loading movie doesn't work!");
    }
}

ofPtr<ofVideoPlayer> ofxTLVideoTrack::getPlayer(){
    return player;
}

void ofxTLVideoTrack::draw(){
	
	if(player == NULL){
		return;
	}
	//inFrame = outFrame = -1;
    inFrame = 0;
    outFrame = player->getTotalNumFrames();

	ofPushStyle();
    
	int selectedFrameX = screenXForTime( timeline->getTimecode().secondsForFrame(selectedFrame));
	
	if(thumbsEnabled && getDrawRect().height > 10){
		//clip hanging frames off the sides

		ofSetColor(255);
        lock();
		for(int i = 0; i < videoThumbs.size(); i++){
            if(videoThumbs[i].thumb != NULL){
                if(videoThumbs[i].loaded &&!videoThumbs[i].thumb->isUsingTexture()){
                    videoThumbs[i].thumb->setUseTexture(true);
                    videoThumbs[i].thumb->update();
                }

                videoThumbs[i].thumb->draw(videoThumbs[i].displayRect);
            }
            else{
                ofPushStyle();
                ofSetColor(0);
                ofRect(videoThumbs[i].displayRect);
                ofPopStyle();
            }
		}
		unlock();
        
		for(int i = 0; i < videoThumbs.size(); i++){

            if (!thumbsEnabled) {
                ofFill();
                ofSetColor(0);
                ofRect(videoThumbs[i].displayRect);
            }
					
            ofNoFill();
            ofSetColor(timeline->getColors().textColor);
            timeline->getFont().drawString(ofToString(videoThumbs[i].framenum), videoThumbs[i].displayRect.x+5, videoThumbs[i].displayRect.y+15);
            ofRect(videoThumbs[i].displayRect);
		}
        
        if(drawVideoPreview){
            ofRectangle previewRect = ofRectangle(0, 0, player->getWidth() - 2, player->getHeight() - 4);
            previewRect.scaleTo(bounds, OF_ASPECT_RATIO_KEEP);
            previewRect.x = selectedFrameX + 1;
            previewRect.y = bounds.y - 1;
            player->draw(previewRect);
            ofPushStyle();
            ofFill();
            ofSetColor(timeline->getColors().backgroundColor, 100);
            ofRect(selectedFrameX + 1, bounds.y + 1, previewRect.width - 2, bounds.height - 2);
            ofPopStyle();
        }
	}
	
	ofSetColor(timeline->getColors().textColor);
	ofLine(selectedFrameX, bounds.y, selectedFrameX, bounds.y+bounds.height);
	timeline->getFont().drawString("F# " + ofToString(selectedFrame), selectedFrameX, bounds.y+15);
	timeline->getFont().drawString(ofxTimecode::timecodeForSeconds(player->getPosition()*player->getDuration()), selectedFrameX, bounds.y+30);
	
	if(inFrame != -1){
		ofSetLineWidth(2);
		ofSetColor(timeline->getColors().highlightColor);
		int inFrameX  = screenXForTime( timeline->getTimecode().secondsForFrame(inFrame));
		int outFrameX = screenXForTime( timeline->getTimecode().secondsForFrame(outFrame));
		ofLine(inFrameX, bounds.y, inFrameX, bounds.y+bounds.height);
		ofLine(outFrameX, bounds.y, outFrameX, bounds.y+bounds.height);
		ofSetColor(timeline->getColors().textColor);
		timeline->getFont().drawString("in  " + ofToString(inFrame),  inFrameX  + 5, bounds.y + 10);
		timeline->getFont().drawString("out " + ofToString(outFrame), outFrameX + 5, bounds.y + bounds.height - 20);
	}
	
	ofPopStyle();
}

void ofxTLVideoTrack::setInFrame(int in){
	inFrame = in;
}

void ofxTLVideoTrack::setOutFrame(int out){
	outFrame = out;
}

bool ofxTLVideoTrack::mousePressed(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mousePressed(args, millis);
	if(isActive()){
		timeline->unselectAll();
		selectFrame( timeline->getTimecode().frameForMillis(millis) );

		if(timeline->getMovePlayheadOnDrag()){
			timeline->setPercentComplete(screenXtoNormalizedX(args.x));
		}
	}
	return false;
}

void ofxTLVideoTrack::mouseMoved(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mouseMoved(args, millis);
}

void ofxTLVideoTrack::mouseDragged(ofMouseEventArgs& args, long millis){
	if(isActive()){
		selectFrame( timeline->getTimecode().frameForMillis(millis) );
		if(timeline->getMovePlayheadOnDrag()){
			timeline->setPercentComplete(screenXtoNormalizedX(args.x));
		}
	}
}

bool ofxTLVideoTrack::isLoaded(){
	return player != NULL && player->isLoaded();
}

void ofxTLVideoTrack::setDrawVideoPreview(bool drawPreview){
    drawVideoPreview = drawPreview;
}

bool ofxTLVideoTrack::getDrawVideoPreview(){
    return drawVideoPreview;
}

//width and height of image elements
float ofxTLVideoTrack::getContentWidth(){
    return isLoaded() ? player->getWidth() : 0;  
}

float ofxTLVideoTrack::getContentHeight(){
    return isLoaded() ? player->getHeight() : 0;    
}


void ofxTLVideoTrack::keyPressed(ofKeyEventArgs& args){
	if(isLoaded() && hasFocus()){
		if(args.key == OF_KEY_LEFT){
			selectFrame(MAX(selectedFrame-1, 0));
			timeline->setCurrentFrame(player->getCurrentFrame());
		}
		else if(args.key == OF_KEY_RIGHT){
			selectFrame(MIN(selectedFrame+1, player->getTotalNumFrames()-1));
			timeline->setCurrentFrame(player->getCurrentFrame());
		}
	}	
}

int ofxTLVideoTrack::selectFrame(int frame){
    if(outFrame - inFrame > 0){
        selectedFrame = inFrame + (frame % (outFrame - inFrame));
    //	cout << "setting frame to " << selectedFrame << " with requested frame " << frame << endl;
        currentLoop = frame / (outFrame-inFrame);
    //	cout << "selecting frame " << selectedFrame << endl;
        player->setFrame(selectedFrame);
        timeline->flagUserChangedValue();
        player->update(); 
    //	cout << "selectFrame: player reports frame " << player->getCurrentFrame() << " with requested frame " << frame << endl;
        //cout << "selecting frame " << frame << " video frame " << selectedFrame << " current loop " << currentLoop << " duration " << player->getTotalNumFrames() << " timeline duration " << timeline->getDurationInFrames() << endl;
    }
	return selectedFrame;
}

void ofxTLVideoTrack::toggleThumbs(){
	thumbsEnabled = !thumbsEnabled;
}

int ofxTLVideoTrack::getCurrentFrame(){
	return (player->getCurrentFrame() - inFrame) + currentLoop*(outFrame-inFrame);
}

float ofxTLVideoTrack::getCurrentTime(){
	//TODO: account for in and out frame.
	return player->getPosition() * player->getDuration();
}

float ofxTLVideoTrack::getDuration(){
	return player->getDuration();
}

int ofxTLVideoTrack::getSelectedFrame(){
	return selectedFrame + currentLoop * (outFrame-inFrame);
}

void ofxTLVideoTrack::exit(ofEventArgs& args){
//    stopThread();
	waitForThread(true);
}

string ofxTLVideoTrack::getTrackType(){
    return "Video";
}
