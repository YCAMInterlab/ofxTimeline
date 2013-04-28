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
//    ofRect(phaseMatchRect);
	ofRect(amplitudeRect);
	ofRect(frequencyRect);
	ofRect(seedRect);
	ofRect(centerRect);
	ofRect(interpolateRect);
    ofRect(expInterpolateRect);
	
	ofNoFill();
	ofSetColor(timeline->getColors().keyColor);
	
	ofRect(sineTypeRect);
	ofRect(noiseTypeRect);
	ofRect(phaseShiftRect);
//    ofRect(phaseMatchRect);
	ofRect(amplitudeRect);
	ofRect(frequencyRect);
	ofRect(seedRect);
	ofRect(centerRect);
	ofRect(interpolateRect);
    ofRect(expInterpolateRect);
	
	ofxTLLFOKey* lfokey = (ofxTLLFOKey*)selectedKeyframe;
	ofSetColor(timeline->getColors().highlightColor, 128);
	ofFill();
	if(lfokey->type == OFXTL_LFO_TYPE_SINE){
		ofRect(sineTypeRect);
	}
	else{
		ofRect(noiseTypeRect);
	}
			//TODO: Phase match
//    if(lfokey->phaseMatch){
//		ofRect(phaseMatchRect);
//	}
    
	if(lfokey->interpolate){
		ofRect(interpolateRect);
	}
    
    if (lfokey->expInterpolate){
        ofRect(expInterpolateRect);
    }
    
	
	ofSetColor(timeline->getColors().textColor, 200);
	float lineHeight = timeline->getFont().getLineHeight();
	timeline->getFont().drawString("sine", sineTypeRect.x+10, sineTypeRect.y+lineHeight);
	timeline->getFont().drawString("noise", noiseTypeRect.x+10, noiseTypeRect.y+lineHeight);
	timeline->getFont().drawString("phase: " + ofToString(lfokey->phaseShift, 1), phaseShiftRect.x+10, phaseShiftRect.y+lineHeight);
			//TODO: Phase match
//	timeline->getFont().drawString("phaseMatch", phaseMatchRect.x+10, phaseMatchRect.y+lineHeight);
	timeline->getFont().drawString("amplitude: " + ofToString(lfokey->amplitude, 4), amplitudeRect.x+10, amplitudeRect.y+lineHeight);
	timeline->getFont().drawString("frequency: " + ofToString(lfokey->frequency, 1), frequencyRect.x+10, frequencyRect.y+lineHeight);
	timeline->getFont().drawString("seed: " + ofToString(lfokey->seed, 1), seedRect.x+10, seedRect.y+lineHeight);
	timeline->getFont().drawString("center: " + ofToString(ofMap(lfokey->center, 0, 4, valueRange.min, valueRange.max), 4), centerRect.x+10, centerRect.y+lineHeight);
	timeline->getFont().drawString("interpolate", interpolateRect.x+10, interpolateRect.y+lineHeight);
	timeline->getFont().drawString("expinterpolate", expInterpolateRect.x+10, expInterpolateRect.y+lineHeight);
	
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

float ofxTLLFO::interpolateValueForKeys(ofxTLKeyframe* start, ofxTLKeyframe* end, unsigned long long sampleTime){
	ofxTLLFOKey* prevKey = (ofxTLLFOKey*)start;
//	prevKey->samplePoint = (1./prevKey->frequency)*(prevKey->phaseShift + prevKey->time + sampleTime );
	
	if(!prevKey->interpolate && !prevKey->expInterpolate){
		return evaluateKeyframeAtTime(prevKey, sampleTime);
	}
	
	ofxTLLFOKey* nextKey = (ofxTLLFOKey*)end;
	
	//parametric interpolation
	if(prevKey->type == nextKey->type){
//      float alpha = sin(2*PI*ofMap(sampleTime, start->time,end->time, 0, 1.0));
//		float alpha = ofMap(sampleTime, start->time,end->time, 0, 1.0);
		ofxTLLFOKey tempkey;
		tempkey.time = prevKey->time;
		tempkey.type = prevKey->type;
		
		tempkey.phaseShift = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->phaseShift, nextKey->phaseShift);
		tempkey.amplitude = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->amplitude, nextKey->amplitude);
		tempkey.center = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->center, nextKey->center);
		tempkey.frequency = ofMap(sampleTime, prevKey->time, nextKey->time, prevKey->frequency, nextKey->frequency);
        
		if (tempkey.type == OFXTL_LFO_TYPE_SINE) {
            
            if (!prevKey->expInterpolate) {
                
                // http://stackoverflow.com/questions/11199509/sine-wave-that-slowly-ramps-up-frequency-from-f1-to-f2-for-a-given-time
                // http://en.wikipedia.org/wiki/Chirp
                
                double interval = (double)(nextKey->time - prevKey->time) / (60.0f * 1000.0f);
                double delta = (double)(sampleTime - prevKey->time) / (double)(nextKey->time - prevKey->time);
                double t = interval * delta;
                double phase = 2.0f * PI * t * (prevKey->frequency + (nextKey->frequency - prevKey->frequency) * delta / 2.0f);
                            
                return ofClamp((cos(phase + prevKey->phaseShift) * tempkey.amplitude * 0.5 + 0.5 + tempkey.center), 0, 1.0);
            
            } else {
                
                // strange if there's no prevKey
                
                double interval = (double)(nextKey->time - prevKey->time) / (60.0f * 1000.0f);
                double k = exp(log(nextKey->frequency / prevKey->frequency) / interval);
                double delta = (double)(sampleTime - prevKey->time) / (double)(nextKey->time - prevKey->time);
                double t = interval * delta;
                double phase = 2 * PI * prevKey->frequency * ((pow(k, t) - 1) / log(k));
                return ofClamp(cos(phase) * 0.5 + 0.5, 0, 1.0);
                
            }
            
        }
        else {
            return evaluateKeyframeAtTime(&tempkey, sampleTime);
        }
	}
	//value interpolation
	else{
		return ofMap(sampleTime, start->time, end->time, evaluateKeyframeAtTime(prevKey, sampleTime), evaluateKeyframeAtTime(nextKey, sampleTime));
	}
}

//the beating heart
float ofxTLLFO::evaluateKeyframeAtTime(ofxTLKeyframe* key, unsigned long long sampleTime, bool firstKey){
    if(firstKey){
        return ofMap(defaultValue, valueRange.min, valueRange.max, 0, 1.0);
    }
	ofxTLLFOKey* lfo = (ofxTLLFOKey*)key;
	if(lfo->type == OFXTL_LFO_TYPE_SINE){
        // when no interpolation needed.
        return ofClamp(( cos( (2.0f*PI * lfo->frequency) * (sampleTime + lfo->phaseShift) / (1000.0f*60.0f) )*lfo->amplitude)*.5 + .5 + lfo->center, 0, 1);
        
	}
	else {
		return ofClamp( (ofSignedNoise(lfo->seed, (2*PI*lfo->frequency/(1000*60*10))*(lfo->phaseShift + sampleTime)) * lfo->amplitude)*.5+.5 + lfo->center, 0, 1);
	}
}

bool ofxTLLFO::mousePressed(ofMouseEventArgs& args, long millis){
	if(drawingLFORect){
		draggedValue = false;
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
			//TODO: Phase match
//        else if(phaseMatchRect.inside(args.x, args.y)){
//			mouseDownRect = &phaseMatchRect;
//		}
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
		else if(expInterpolateRect.inside(args.x, args.y)){
			mouseDownRect = &expInterpolateRect;
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
			draggedValue = true;
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
			//TODO: Phase match
//            else if(mouseDownRect == &phaseMatchRect){
//				lfokey->phaseMatch = !lfokey->phaseMatch;
//				shouldRecomputePreviews = true;
//				timeline->flagTrackModified(this);
//			}
			else if(mouseDownRect == &interpolateRect){
				lfokey->interpolate = !lfokey->interpolate;
                if (lfokey->interpolate) lfokey->expInterpolate = false;
				shouldRecomputePreviews = true;
				timeline->flagTrackModified(this);
			}
			else if(mouseDownRect == &expInterpolateRect){
				lfokey->expInterpolate = !lfokey->expInterpolate;
                if (lfokey->expInterpolate) lfokey->interpolate = false;
				shouldRecomputePreviews = true;
				timeline->flagTrackModified(this);
			}
			
			if(draggedValue){
				timeline->flagTrackModified(this);
				draggedValue = false;
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
    newKey->phaseMatch = false;
	newKey->amplitude = 1.0;
	newKey->frequency = 100.;
	newKey->center = 0 ; // 0 is middle
	newKey->interpolate = true;
    newKey->expInterpolate = false;
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
	lfoKey->expInterpolate = xmlStore.getValue("expInterpolate", true);
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
	xmlStore.addValue("expInterpolate",lfoKey->expInterpolate);
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
			//TODO: Phase match	
//	phaseMatchRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
//	rectNum++;
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
	expInterpolateRect = ofRectangle(rectangleX, rectangleY+rectNum*rectHeight, rectWidth, rectHeight);
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
