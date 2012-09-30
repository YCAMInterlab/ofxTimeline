
#include "ofxTLLFO.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"

ofxTLLFO::ofxTLLFO(){
	drawingLFORect = false;
}

ofxTLLFO::~ofxTLLFO(){
	
}

//draw your keyframes into bounds
void ofxTLLFO::draw(){
	
	//we draw keys our own way

	//ofxTLKeyframes::draw();
	if(bounds.width == 0 || bounds.height < 2){
		return;
	}
	
	if(shouldRecomputePreviews || viewIsDirty){
		recomputePreviews();
	}

	ofSetColor(timeline->getColors().keyColor);
	preview.draw();

	if(drawingLFORect){
		ofPushStyle();
		ofSetColor(timeline->getColors().disabledColor, 150);
		
		ofFill();
		ofRect(lfoRect);
		ofNoFill();
		ofSetColor(timeline->getColors().keyColor);
		ofRect(lfoRect);
		
		ofPopStyle();
	}

	
	ofPushStyle();
	for(int i = 0; i < keyframes.size(); i++){
		//make sure it's on screen
		if(isKeyframeIsInBounds(keyframes[i])){
			//we know the type because we created it in newKeyframe()
			//so we can safely cast
			ofxTLLFOKey* lfoKey = (ofxTLLFOKey*)keyframes[i];

			if(isKeyframeSelected(keyframes[i])){
				ofSetLineWidth(2);
				ofSetColor(timeline->getColors().textColor);
			}
			else if(keyframes[i] == hoverKeyframe){
				ofSetLineWidth(4);
				ofSetColor(timeline->getColors().highlightColor);
			}
			else{
				ofSetLineWidth(4);
				ofSetColor(timeline->getColors().keyColor);
			}
			float screenX = millisToScreenX(keyframes[i]->time);
			ofLine(screenX, bounds.y, screenX, bounds.y+bounds.height);
		}
	}
	
	ofPopStyle();
}

float ofxTLLFO::interpolateValueForKeys(ofxTLKeyframe* start, ofxTLKeyframe* end, unsigned long sampleTime){
	ofxTLLFOKey* prevKey = (ofxTLLFOKey*)start;
	if(!prevKey->interpolate){
		return evaluateKeyframeAtTime(prevKey, sampleTime);
	}
	
	ofxTLLFOKey* nextKey = (ofxTLLFOKey*)end;
//	float alpha = ofMap(sampleTime, start->time,end->time, 0, 1.0);
	//parametric interpolation
	if(prevKey->type == nextKey->type){
		ofxTLLFOKey tempkey;
		tempkey.type = prevKey->type;
		tempkey.phaseShift = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->phaseShift, nextKey->phaseShift);
		tempkey.amplitude = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->amplitude, nextKey->amplitude);
		tempkey.center = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->center, nextKey->center);
		tempkey.frequency = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->frequency, nextKey->frequency);
		return evaluateKeyframeAtTime(&tempkey, sampleTime);
	}
	//value interpolation
	else{
		return ofMap(sampleTime, start->time, end->time, evaluateKeyframeAtTime(prevKey, sampleTime), evaluateKeyframeAtTime(nextKey, sampleTime));
	}
}

float ofxTLLFO::evaluateKeyframeAtTime(ofxTLKeyframe* key, unsigned long sampleTime){
	ofxTLLFOKey* lfo = (ofxTLLFOKey*)key;
	if(lfo->type == OFXTL_LFO_TYPE_SINE){
		return ofClamp( (sin(lfo->frequency*(lfo->phaseShift + sampleTime))*.5+.5) * lfo->amplitude + lfo->center, 0, 1);
	}
	else {
		return ofClamp( (ofNoise(lfo->seed, lfo->frequency*(lfo->phaseShift + sampleTime))) * lfo->amplitude + lfo->center, 0, 1);
	}
}

bool ofxTLLFO::mousePressed(ofMouseEventArgs& args, long millis){
	//for the general behavior call the super class
	//or you can do your own thing. Return true if the click caused an item to
	//become selected
	return ofxTLKeyframes::mousePressed(args, millis);
}

void ofxTLLFO::mouseMoved(ofMouseEventArgs& args, long millis){
	if(drawingLFORect){
	}
	else{
		ofxTLKeyframes::mouseMoved(args, millis);
	}
}

void ofxTLLFO::mouseDragged(ofMouseEventArgs& args, long millis){
	if(drawingLFORect){
	}
	else{
		ofxTLKeyframes::mouseDragged(args, millis);
	}
}

void ofxTLLFO::mouseReleased(ofMouseEventArgs& args, long millis){
	if(drawingLFORect){
		if(args.button == 0 && !ofGetModifierControlPressed() && !lfoRect.inside(args.x, args.y)){
			timeline->dismissedModalContent();
			drawingLFORect = false;
		}
	}
	else{
		ofxTLKeyframes::mouseReleased(args, millis);
	}
}

//keys pressed events, and nuding from arrow keys with normalized nudge amount 0 - 1.0
void ofxTLLFO::keyPressed(ofKeyEventArgs& args){
	ofxTLKeyframes::keyPressed(args);
}

string ofxTLLFO::getTrackType(){
	return "LFO";
}

ofxTLKeyframe* ofxTLLFO::newKeyframe(){
	//return our type of keyframe, stored in the parent class
	ofxTLLFOKey* newKey = new ofxTLLFOKey();
	newKey->type = OFXTL_LFO_TYPE_NOISE;
	newKey->phaseShift = 0; //in millis
	newKey->amplitude = 1.0;
	newKey->frequency = .001;
	newKey->center = 0 ; // 0 is middle
	newKey->interpolate = true;
	newKey->seed = 0;
	return newKey;
}

void ofxTLLFO::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLLFOKey* lfoKey = (ofxTLLFOKey*)key;
	lfoKey->type = (ofxTLLFOType)xmlStore.getValue("type", int(OFXTL_LFO_TYPE_NOISE));
	lfoKey->phaseShift = xmlStore.getValue("phaseShift", 0);
	lfoKey->amplitude = xmlStore.getValue("amplitude", 1.);
	lfoKey->frequency = xmlStore.getValue("frequency", .001);
	lfoKey->seed = xmlStore.getValue("seed", 0);
	lfoKey->center = xmlStore.getValue("center", 0);
	lfoKey->interpolate = xmlStore.getValue("interpolate", true);
}

void ofxTLLFO::storeKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLLFOKey* lfoKey = (ofxTLLFOKey*)key;
	ofxTLLFOType type;
	xmlStore.addValue("type", int(lfoKey->type));
	xmlStore.addValue("phaseShift",lfoKey->phaseShift);
	xmlStore.addValue("amplitude",lfoKey->amplitude);
	xmlStore.addValue("frequency",lfoKey->frequency);
	xmlStore.addValue("seed",lfoKey->seed);
	xmlStore.addValue("center",lfoKey->center);
	xmlStore.addValue("interpolate",lfoKey->interpolate);

}

void ofxTLLFO::selectedKeySecondaryClick(ofMouseEventArgs& args){
	drawingLFORect = true;
	lfoRect = ofRectangle(args.x,args.y, 40,40);
	timeline->presentedModalContent(this);
	
	//you can make a popup window start here

	//and then when you want to get rid of it call somewhere else
//	timeline->dismissedModalContent();
	//this will lock all other timeline interaction and feed all things into your track
	//so feel free to draw out of bounds, but don't go off the screen or out of the timeline
}

void ofxTLLFO::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
    	if(timeRange.contains( keyframes[i]->time )){
            selectKeyframe(keyframes[i]);
        }
	}
}

ofxTLKeyframe* ofxTLLFO::keyframeAtScreenpoint(ofVec2f p){
    if(bounds.inside(p.x, p.y)){
        for(int i = 0; i < keyframes.size(); i++){
            float offset = p.x - timeline->millisToScreenX(keyframes[i]->time);
            if (abs(offset) < 5) {
                return keyframes[i];
            }
        }
    }
	return NULL;
}


void ofxTLLFO::willDeleteKeyframe(ofxTLKeyframe* keyframe){
	//do any cleanup before this keyframe gets hosed
}
