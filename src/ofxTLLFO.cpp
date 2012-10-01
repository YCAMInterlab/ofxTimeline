
#include "ofxTLLFO.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"

ofxTLLFO::ofxTLLFO(){
	drawingLFORect = false;
	rectWidth = 120;
	rectHeight = 15;
	mouseDownRect = NULL;
	editingParam = NULL;
	drawingLFORect = false;
}

ofxTLLFO::~ofxTLLFO(){
	
}

void ofxTLLFO::drawModalContent(){
	if(!drawingLFORect){
		return;
	}
	
	ofPushStyle();
	ofSetColor(timeline->getColors().disabledColor, 200);
	
	ofSetLineWidth(1);
	ofFill();
	
	ofRect(sineTypeRect);
	ofRect(noiseTypeRect);
	ofRect(phaseShiftRect);
	ofRect(amplitudeRect);
	ofRect(frequencyRect);
	ofRect(seedRect);
	ofRect(centerRect);
	ofRect(interpolateRect);
	
	ofNoFill();
	ofSetColor(timeline->getColors().keyColor);
	
	ofRect(sineTypeRect);
	ofRect(noiseTypeRect);
	ofRect(phaseShiftRect);
	ofRect(amplitudeRect);
	ofRect(frequencyRect);
	ofRect(seedRect);
	ofRect(centerRect);
	ofRect(interpolateRect);
	
	ofxTLLFOKey* lfokey = (ofxTLLFOKey*)selectedKeyframe;
	ofSetColor(timeline->getColors().highlightColor, 128);
	ofFill();
	if(lfokey->type == OFXTL_LFO_TYPE_SINE){
		ofRect(sineTypeRect);
	}
	else{
		ofRect(noiseTypeRect);
	}
	if(lfokey->interpolate){
		ofRect(interpolateRect);
	}
	
	ofSetColor(timeline->getColors().textColor, 200);
	float lineHeight = timeline->getFont().getLineHeight();
	timeline->getFont().drawString("sine", sineTypeRect.x+10, sineTypeRect.y+lineHeight);
	timeline->getFont().drawString("noise", noiseTypeRect.x+10, noiseTypeRect.y+lineHeight);
	timeline->getFont().drawString("phase: " + ofToString(lfokey->phaseShift, 1), phaseShiftRect.x+10, phaseShiftRect.y+lineHeight);
	timeline->getFont().drawString("amplitude: " + ofToString(lfokey->amplitude, 4), amplitudeRect.x+10, amplitudeRect.y+lineHeight);
	timeline->getFont().drawString("frequency: " + ofToString(lfokey->frequency, 1), frequencyRect.x+10, frequencyRect.y+lineHeight);
	timeline->getFont().drawString("seed: " + ofToString(lfokey->seed, 1), seedRect.x+10, seedRect.y+lineHeight);
	timeline->getFont().drawString("center: " + ofToString(ofMap(lfokey->center, 0, 4, valueRange.min, valueRange.max), 4), centerRect.x+10, centerRect.y+lineHeight);
	timeline->getFont().drawString("interpolate", interpolateRect.x+10, interpolateRect.y+lineHeight);
	
	ofPopStyle();
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
	
	ofSetColor(timeline->getColors().disabledColor, 30);
	float currentPercent = sampleAtTime(currentTrackTime());
	ofFill();
	ofRect(bounds.x, bounds.getMaxY(), bounds.width, -bounds.height*currentPercent);
	
	ofPushStyle();
	ofSetColor(timeline->getColors().keyColor);
	preview.draw();
	
	
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
//	prevKey->samplePoint = (1./prevKey->frequency)*(prevKey->phaseShift + prevKey->time + sampleTime );
	
	if(!prevKey->interpolate){
		return evaluateKeyframeAtTime(prevKey, sampleTime);
	}
	
	ofxTLLFOKey* nextKey = (ofxTLLFOKey*)end;
	
	//parametric interpolation
	if(prevKey->type == nextKey->type){
		//float alpha = sin(2*PI*ofMap(sampleTime, start->time,end->time, 0, 1.0));
//		float alpha = ofMap(sampleTime, start->time,end->time, 0, 1.0);
		ofxTLLFOKey tempkey;
		tempkey.time = prevKey->time;
		tempkey.type = prevKey->type;
		tempkey.phaseShift = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->phaseShift, nextKey->phaseShift);
		tempkey.amplitude = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->amplitude, nextKey->amplitude);
		tempkey.center = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->center, nextKey->center);
		tempkey.frequency = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->frequency, nextKey->frequency);
		tempkey.freqDeviation = (nextKey->frequency - prevKey->frequency);
		//ofxTLLFOKey* lfo = &tempkey;
		//return ofClamp( (cos( (2*PI*lfo->frequency/(1000*60))*(sampleTime+lfo->phaseShift) )*lfo->amplitude)*.5 + .5 + lfo->center, 0, 1);
//		alpha = 1;
//		return ofClamp( (cos( 2*PI*(prevKey->frequency * prevKey->time)/(1000*60) +
//							  2*PI*(prevKey->frequency + tempkey.freqDeviation*alpha)*(sampleTime-prevKey->time)/(1000*60) )*tempkey.amplitude)*.5 + .5 + tempkey.center, 0, 1);
//		return ofClamp( (cos( (2*PI*(prevKey->frequency)/(1000*60)*sampleTime) + tempkey.freqDeviation/*cos(2*) alpha)) )*tempkey.amplitude)*.5 + .5 + tempkey.center, 0, 1);
		//return ofClamp( (sin( (2*PI*prevKey->frequency/(1000*60))*(sampleTime) + 2*PI*(tempkey.freqDeviation/(1000*60)*sampleTime)*alpha )*tempkey.amplitude)*.5 + .5 + tempkey.center, 0, 1);
		return evaluateKeyframeAtTime(&tempkey, sampleTime);
	}
	//value interpolation
	else{
		return ofMap(sampleTime, start->time, end->time, evaluateKeyframeAtTime(prevKey, sampleTime), evaluateKeyframeAtTime(nextKey, sampleTime));
	}
}

//the beating heart
float ofxTLLFO::evaluateKeyframeAtTime(ofxTLKeyframe* key, unsigned long sampleTime){
	ofxTLLFOKey* lfo = (ofxTLLFOKey*)key;
	if(lfo->type == OFXTL_LFO_TYPE_SINE){
		//return ofClamp( (sin( lfo->samplePoint )*lfo->amplitude)*.5 + .5 + lfo->center, 0, 1);
		return ofClamp( (cos( (2*PI*lfo->frequency/(1000*60))*(sampleTime+lfo->phaseShift) )*lfo->amplitude)*.5 + .5 + lfo->center, 0, 1);
	}
	else {
		return ofClamp( (ofSignedNoise(lfo->seed, (2*PI*lfo->frequency/(1000*60*10))*(lfo->phaseShift + sampleTime)) * lfo->amplitude)*.5+.5 + lfo->center, 0, 1);
	}
}

bool ofxTLLFO::mousePressed(ofMouseEventArgs& args, long millis){
	if(drawingLFORect){
		mouseDownRect = NULL;
		editingParam = NULL;
		editingClickX = args.x;
		if(sineTypeRect.inside(args.x, args.y)){
			mouseDownRect = &sineTypeRect;
		}
		else if(noiseTypeRect.inside(args.x, args.y)){
			mouseDownRect = &noiseTypeRect;
		}
		else if(phaseShiftRect.inside(args.x, args.y)){
			mouseDownRect = &phaseShiftRect;
			editingParam = &((ofxTLLFOKey*)selectedKeyframe)->phaseShift;
			editingRange = ofRange(-30000, 30000);
			editingExponent = 1;
			editingSensitivity = 1;
		}
		else if(amplitudeRect.inside(args.x, args.y)){
			mouseDownRect = &amplitudeRect;
			editingParam = &((ofxTLLFOKey*)selectedKeyframe)->amplitude;
			editingRange = ofRange(0, 100);
			editingExponent = 2;
			editingSensitivity = .001;
		}
		else if(frequencyRect.inside(args.x, args.y)){
			mouseDownRect = &frequencyRect;
			editingParam = &((ofxTLLFOKey*)selectedKeyframe)->frequency;
			editingRange = ofRange(1, 10000);
			editingExponent = 2;
			editingSensitivity = .1;
		}
		else if(seedRect.inside(args.x, args.y)){
			mouseDownRect = &seedRect;
			editingParam = &((ofxTLLFOKey*)selectedKeyframe)->seed;
			editingRange = ofRange(0, 1000);
			editingExponent = 1;
			editingSensitivity = .1;
		}
		else if(centerRect.inside(args.x, args.y)){
			mouseDownRect = &centerRect;
			editingParam = &((ofxTLLFOKey*)selectedKeyframe)->center;
			editingRange = ofRange(-2, 2);
			editingExponent = 2;
			editingSensitivity = .001;
		}
		else if(interpolateRect.inside(args.x, args.y)){
			mouseDownRect = &interpolateRect;
		}
		if(editingParam != NULL){
			editingStartValue = *editingParam;
		}
	}
	else{
		return ofxTLKeyframes::mousePressed(args, millis);
	}
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
		if(mouseDownRect != NULL && editingParam != NULL){
			float delta = (args.x-editingClickX)*editingSensitivity;
			*editingParam = ofClamp(editingStartValue + delta, editingRange.min, editingRange.max);
			shouldRecomputePreviews = true;
			timeline->flagUserChangedValue();
		}
	}
	else{
		ofxTLKeyframes::mouseDragged(args, millis);
	}
}

void ofxTLLFO::mouseReleased(ofMouseEventArgs& args, long millis){
	if(drawingLFORect){
		if(mouseDownRect != NULL && mouseDownRect->inside(args.x, args.y)){
			ofxTLLFOKey* lfokey = (ofxTLLFOKey*)selectedKeyframe;
			if(mouseDownRect == &sineTypeRect){
				if( lfokey->type != OFXTL_LFO_TYPE_SINE){
					lfokey->type = OFXTL_LFO_TYPE_SINE;
					shouldRecomputePreviews = true;
					timeline->flagTrackModified(this);
				}
			}
			else if(mouseDownRect == &noiseTypeRect){
				if( lfokey->type != OFXTL_LFO_TYPE_NOISE){
					lfokey->type = OFXTL_LFO_TYPE_NOISE;
					shouldRecomputePreviews = true;
					timeline->flagTrackModified(this);
				}
			}
			else if(mouseDownRect == &interpolateRect){
				lfokey->interpolate = !lfokey->interpolate;
				shouldRecomputePreviews = true;
				timeline->flagTrackModified(this);
			}
		}
		if(args.button == 0 && !ofGetModifierControlPressed() && !lfoRect.inside(args.x, args.y) && mouseDownRect == NULL){
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
	newKey->type = OFXTL_LFO_TYPE_SINE;
	newKey->phaseShift = 0; //in millis
	newKey->amplitude = 1.0;
	newKey->frequency = 100.;
	newKey->center = 0 ; // 0 is middle
	newKey->interpolate = true;
	newKey->seed = 0;
	return newKey;
}

void ofxTLLFO::restoreKeyframe(ofxTLKeyframe* key, ofxXmlSettings& xmlStore){
	ofxTLLFOKey* lfoKey = (ofxTLLFOKey*)key;
	lfoKey->type = (ofxTLLFOType)xmlStore.getValue("type", int(OFXTL_LFO_TYPE_NOISE));
	lfoKey->phaseShift = xmlStore.getValue("phaseShift", 0.);
	lfoKey->amplitude = xmlStore.getValue("amplitude", 1.);
	lfoKey->frequency = xmlStore.getValue("frequency", 100.);
	lfoKey->seed = xmlStore.getValue("seed", 0.);
	lfoKey->center = xmlStore.getValue("center", 0.);
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

	int totalDrawHeight =(7*rectHeight);
	int rectangleY;
	rectangleY = bounds.y;
	if(rectangleY+totalDrawHeight > timeline->getDrawRect().getMaxY() ){
		rectangleY -= totalDrawHeight;
	}
	
	int rectangleX = args.x - rectWidth;
	if(rectangleX < timeline->getDrawRect().getMinX()){
		rectangleX = timeline->getDrawRect().getMinX();
	}
	int rectNum = 0;
	sineTypeRect = ofRectangle(rectangleX, rectangleY, rectWidth/2, rectHeight);
	noiseTypeRect = ofRectangle(rectangleX+rectWidth/2, rectangleY, rectWidth/2, rectHeight);
	rectNum++;
	phaseShiftRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
	rectNum++;
	amplitudeRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
	rectNum++;
	frequencyRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
	rectNum++;
	seedRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
	rectNum++;
	centerRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
	rectNum++;
	interpolateRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
	rectNum++;

	lfoRect = ofRectangle(rectangleX, rectangleY, rectWidth, totalDrawHeight);
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
