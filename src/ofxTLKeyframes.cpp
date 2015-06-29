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

#include "ofxTLKeyframes.h"
#include "ofxTimeline.h"
#include "ofxHotKeys.h"

bool keyframesort(ofxTLKeyframe* a, ofxTLKeyframe* b){
	return a->time < b->time;
}

ofxTLKeyframes::ofxTLKeyframes()
:	hoverKeyframe(NULL),
	keysAreDraggable(false),
	keysDidDrag(false),
	keysDidNudge(false),
	lastKeyframeIndex(1),
	lastSampleTime(0),
	shouldRecomputePreviews(false),
	createNewOnMouseup(false),
	useBinarySave(false),
	valueRange(ofRange(0,1.))
{
	xmlFileName = "_keyframes.xml";	
}

ofxTLKeyframes::~ofxTLKeyframes(){
	clear();
}

void ofxTLKeyframes::recomputePreviews(){
	preview.clear();
	
//	cout << "ofxTLKeyframes::recomputePreviews " << endl;
	
//	if(keyframes.size() == 0 || keyframes.size() == 1){
//		preview.addVertex(ofPoint(bounds.x, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height));
//		preview.addVertex(ofPoint(bounds.x+bounds.width, bounds.y + bounds.height - sampleAtPercent(.5f)*bounds.height));
//	}
//	else{
		for(int p = bounds.getMinX(); p <= bounds.getMaxX(); p+=2){
			preview.addVertex(p,  bounds.y + bounds.height - sampleAtPercent(screenXtoNormalizedX(p)) * bounds.height);
		}
//	}
//	int size = preview.getVertices().size();
	preview.simplify();
	//cout << "simplify pre " << size << " post: " << preview.getVertices().size() << " dif: " << (size - preview.getVertices().size()) << endl;

	ofVec2f lastPoint;
	keyPoints.clear();
	for(int i = 0; i < keyframes.size(); i++){
		if(!isKeyframeIsInBounds(keyframes[i])){
			continue;
		}
		ofVec2f screenpoint = screenPositionForKeyframe(keyframes[i]);
		if(lastPoint.squareDistance(screenpoint) > 5*5){
			keyPoints.push_back(screenpoint);
		}
		
		lastPoint = screenpoint;
	}
	
	shouldRecomputePreviews = false;
	
}

void ofxTLKeyframes::draw(){
	
	if(bounds.width == 0 || bounds.height < 2){
		return;
	}
	
	if(shouldRecomputePreviews || viewIsDirty){
		recomputePreviews();
	}
	
	ofPushStyle();
	
	//draw current value indicator as a big transparent rectangle
	ofSetColor(timeline->getColors().disabledColor, 30);
	//jg play solo change
	//float currentPercent = sampleAtTime(timeline->getCurrentTimeMillis());
	float currentPercent = sampleAtTime(currentTrackTime());
	ofFill();
	ofRect(bounds.x, bounds.getMaxY(), bounds.width, -bounds.height*currentPercent);
	
	//***** DRAW KEYFRAME LINES
	ofSetColor(timeline->getColors().keyColor);
	ofNoFill();
	
	preview.draw();
	
	//**** DRAW KEYFRAME DOTS
	
	//**** HOVER FRAME
	if(hoverKeyframe != NULL){
		ofPushStyle();
		ofFill();
		ofSetColor(timeline->getColors().highlightColor);
		ofVec2f hoverKeyPoint = screenPositionForKeyframe( hoverKeyframe );
		ofCircle(hoverKeyPoint.x, hoverKeyPoint.y, 6);
		ofPopStyle();
	}

	//**** ALL CACHED VISIBLE KEYS
	ofSetColor(timeline->getColors().textColor);
	ofNoFill();
	for(int i = 0; i < keyPoints.size(); i++){
		ofRect(keyPoints[i].x-1, keyPoints[i].y-1, 3, 3);
	}
	
	//**** SELECTED KEYS
	ofSetColor(timeline->getColors().textColor);
	ofFill();
	for(int i = 0; i < selectedKeyframes.size(); i++){
		if(isKeyframeIsInBounds(selectedKeyframes[i])){
			ofVec2f screenpoint = screenPositionForKeyframe(selectedKeyframes[i]);
			float keysValue = ofMap(selectedKeyframes[i]->value, 0, 1.0, valueRange.min, valueRange.max, true);
			if(keysAreDraggable){
				string frameString = timeline->formatTime(selectedKeyframes[i]->time);
				timeline->getFont().drawString(ofToString(keysValue, 4), screenpoint.x+5, screenpoint.y-5);
			}
			ofCircle(screenpoint.x, screenpoint.y, 4);
		}
	}

	ofPopStyle();
}

//TODO: potentially scale internal values at this point
void ofxTLKeyframes::setValueRange(ofRange range, float newDefaultValue){
	valueRange = range;
    defaultValue = newDefaultValue;
}

void ofxTLKeyframes::setValueRangeMin(float min){
	valueRange.min = min;
}

void ofxTLKeyframes::setValueRangeMax(float max){
	valueRange.max = max;
}

void ofxTLKeyframes::setDefaultValue(float newDefaultValue){
	defaultValue = newDefaultValue;
}

void ofxTLKeyframes::quantizeKeys(int step){
	for(int i = 0; i < keyframes.size(); i++){
		setKeyframeTime(keyframes[i], getTimeline()->getQuantizedTime(keyframes[i]->time, step));
	}
}

ofRange ofxTLKeyframes::getValueRange(){
	return valueRange;
}

float ofxTLKeyframes::getValue(){
	return getValueAtTimeInMillis(currentTrackTime());
}

//main function to get values out of the timeline, operates on the given value range
float ofxTLKeyframes::getValueAtPercent(float percent){
	//	return ofMap(sampleAt(percent), 0.0, 1.0, valueRange.min, valueRange.max, false);
    return getValueAtTimeInMillis(percent*timeline->getDurationInMilliseconds());
}

float ofxTLKeyframes::getValueAtTimeInMillis(long sampleTime){
	return ofMap(sampleAtTime(sampleTime), 0.0, 1.0, valueRange.min, valueRange.max, false);
}

float ofxTLKeyframes::sampleAtPercent(float percent){
	return sampleAtTime(percent * timeline->getDurationInMilliseconds());
}

float ofxTLKeyframes::sampleAtTime(long sampleTime){
	sampleTime = ofClamp(sampleTime, 0, timeline->getDurationInMilliseconds());
	
	//edge cases
	if(keyframes.size() == 0){
		return ofMap(defaultValue, valueRange.min, valueRange.max, 0, 1.0, true);
	}
	
	if(sampleTime <= keyframes[0]->time){
		return evaluateKeyframeAtTime(keyframes[0], sampleTime, true);
	}
	
	if(sampleTime >= keyframes[keyframes.size()-1]->time){
		return evaluateKeyframeAtTime(keyframes[keyframes.size()-1], sampleTime);
	}
	
	//optimization for linear playback
	int startKeyframeIndex = 1;
	if(sampleTime >= lastSampleTime){
		startKeyframeIndex = lastKeyframeIndex;
	}
	
	for(int i = startKeyframeIndex; i < keyframes.size(); i++){
		if(keyframes[i]->time >= sampleTime){
			lastKeyframeIndex = i;
			lastSampleTime = sampleTime;
			return interpolateValueForKeys(keyframes[i-1], keyframes[i], sampleTime);
		}
	}
	ofLog(OF_LOG_ERROR, "ofxTLKeyframes --- Error condition, couldn't find keyframe for percent " + ofToString(sampleTime));
	return defaultValue;
}

float ofxTLKeyframes::evaluateKeyframeAtTime(ofxTLKeyframe* key, unsigned long long sampleTime, bool firstKey){
	return key->value;
}

float ofxTLKeyframes::interpolateValueForKeys(ofxTLKeyframe* start,ofxTLKeyframe* end, unsigned long long sampleTime){
	return ofMap(sampleTime, start->time, end->time, start->value, end->value);
}

void ofxTLKeyframes::load(){
    clear();
	if(useBinarySave){
		loadFromBinaryFile();
	}
	else{
		ofxXmlSettings savedkeyframes;
		if(!savedkeyframes.loadFile(xmlFileName)){
//			ofLog(OF_LOG_NOTICE, "ofxTLKeyframes --- couldn't load xml file " + xmlFileName);
			return;
		}
		
		createKeyframesFromXML(savedkeyframes, keyframes);
	}
	updateKeyframeSort();
	timeline->flagTrackModified(this);
}

void ofxTLKeyframes::createKeyframesFromXML(ofxXmlSettings xmlStore, vector<ofxTLKeyframe*>& keyContainer){

	int numKeyframeStores = xmlStore.getNumTags("keyframes");
	for(int store = 0; store < numKeyframeStores; store++){
		xmlStore.pushTag("keyframes",store);
		int numKeyTags = xmlStore.getNumTags("key");
		
		for(int i = 0; i < numKeyTags; i++){
			xmlStore.pushTag("key", i);
			//ofxTLKeyframe* key = newKeyframe(ofVec2f(xmlStore.getValue("x", 0.0),xmlStore.getValue("y", 0.0)));
            ofxTLKeyframe* key = newKeyframe();
            
            string legacyX = xmlStore.getValue("x", "");
            //if there is a decimal this is most likely an old save so let's 
            //convert it based on the current duration
            if(legacyX != ""){
                ofLogNotice() << "ofxTLKeyframes::createKeyframesFromXML -- Found legacy time " + legacyX << endl;
                float normalizedTime = ofToFloat(legacyX);
                key->time = key->previousTime =  normalizedTime*timeline->getDurationInMilliseconds();
            }
            else {
                string timecode = xmlStore.getValue("time", "00:00:00:000");
	            key->time = key->previousTime = timeline->getTimecode().millisForTimecode(timecode);
            }
            
            float legacyYValue = xmlStore.getValue("y", 0.0);
            if(legacyYValue != 0.0){
                ofLogNotice() << "ofxTLKeyframes::createKeyframesFromXML -- Found legacy value " << legacyYValue << endl;
                key->value = legacyYValue;
            }
            else{
	            key->value = xmlStore.getValue("value", 0.0f);
            }
			restoreKeyframe(key, xmlStore);			
			xmlStore.popTag(); //key
			keyContainer.push_back( key );
		}
		
		xmlStore.popTag(); //keyframes
	}
	sort(keyContainer.begin(), keyContainer.end(), keyframesort);
}

void ofxTLKeyframes::clear(){

	for(int i = 0; i < keyframes.size(); i++){
		willDeleteKeyframe(keyframes[i]);
		delete keyframes[i];
	}
	keyframes.clear();
    selectedKeyframes.clear();
	updateKeyframeSort();
}

void ofxTLKeyframes::save(){
	if(useBinarySave){
		saveToBinaryFile();
	}
	else{
		string xmlRep = getXMLStringForKeyframes(keyframes);
		ofxXmlSettings savedkeyframes;
		savedkeyframes.loadFromBuffer(xmlRep);
		savedkeyframes.saveFile(xmlFileName);
	}
}

string ofxTLKeyframes::getXMLStringForKeyframes(vector<ofxTLKeyframe*>& keys){
//	return "";
	ofxXmlSettings savedkeyframes;
	savedkeyframes.addTag("keyframes");
	savedkeyframes.pushTag("keyframes");

	for(int i = 0; i < keys.size(); i++){
		savedkeyframes.addTag("key");
		savedkeyframes.pushTag("key", i);
        
        //calling store before saving the default values gives the subclass a chance to modify them
        storeKeyframe(keys[i], savedkeyframes);
        savedkeyframes.addValue("time", ofxTimecode::timecodeForMillis(keys[i]->time));
		savedkeyframes.addValue("value", keys[i]->value);
        
		savedkeyframes.popTag(); //key
	}

	savedkeyframes.popTag();//keyframes
	string str;
	savedkeyframes.copyXmlToString(str);
	return str;
}

bool ofxTLKeyframes::mousePressed(ofMouseEventArgs& args, long millis){
	
	ofVec2f screenpoint = ofVec2f(args.x, args.y);
	keysAreStretchable = ofGetModifierShiftPressed() && ofGetModifierControlPressed();
    keysDidDrag = false;
	if(keysAreStretchable && timeline->getTotalSelectedItems() > 1){
		unsigned long long minSelected = timeline->getEarliestSelectedTime();
		unsigned long long maxSelected = timeline->getLatestSelectedTime();
		if(minSelected == maxSelected){
			keysAreStretchable = false;
		}
		else {
			unsigned long long midSelection = (maxSelected-minSelected)/2 + minSelected;
			//the anchor is the selected key opposite to where we are stretching
			stretchAnchor = midSelection <= millis ? minSelected : maxSelected;
//			cout << "Min selected " << ofxTimecode::timecodeForMillis(minSelected) << " Mid Selected " << ofxTimecode::timecodeForMillis(midSelection) << " Max selected " << ofxTimecode::timecodeForMillis(maxSelected) << " anchor "  << ofxTimecode::timecodeForMillis(stretchAnchor) << " millis down " << ofxTimecode::timecodeForMillis(millis) << endl;
			stretchSelectPoint = millis;
			//don't do anything else, like create or deselect keyframes
			updateStretchOffsets(screenpoint, millis);
		}
		return true;
	}
	
    keysAreDraggable = !ofGetModifierShiftPressed();
	selectedKeyframe =  keyframeAtScreenpoint(screenpoint);
    //if we clicked OFF of a keyframe OR...
    //if we clicked on a keyframe outside of the current selection and we aren't holding down shift, clear all
    if(!ofGetModifierSelection() && (isActive() || selectedKeyframe != NULL) ){
        bool didJustDeselect = false;
	    if( selectedKeyframe == NULL || !isKeyframeSelected(selectedKeyframe)){
            //settings this to true causes the first click off of the timeline to deselct rather than create a new keyframe
            didJustDeselect = timeline->getTotalSelectedItems() > 1;
    	    timeline->unselectAll();
        }

        //if we didn't just deselect everything and clicked in an empty space add a new keyframe there
        if(selectedKeyframe == NULL && !didJustDeselect){
			createNewOnMouseup = args.button == 0 && !ofGetModifierControlPressed();
        }
    }

	if(selectedKeyframe != NULL){
         //add the keyframe to the selection, whether it was just generated or not
    	if(!isKeyframeSelected(selectedKeyframe)){
			selectedKeyframes.push_back(selectedKeyframe);
			updateKeyframeSort();
//			selectKeyframe(selectedKeyframe);
        }
        //unselect it if it's selected and we clicked the key with shift pressed
        else if(ofGetModifierSelection()){
        	deselectKeyframe(selectedKeyframe);
			selectedKeyframe = NULL;
        }
	}
	
    //if we have any keyframes selected update the grab offsets and check for showing the modal window
	if(selectedKeyframes.size() != 0){
        updateDragOffsets(screenpoint, millis);
		if(selectedKeyframe != NULL){

			if(args.button == 0 && !ofGetModifierSelection() && !ofGetModifierControlPressed()){

	            timeline->setDragTimeOffset(selectedKeyframe->grabTimeOffset);
				//move the playhead
				if(timeline->getMovePlayheadOnDrag()){
					timeline->setCurrentTimeMillis(selectedKeyframe->time);
				}
			}
			if(args.button == 2 || ofGetModifierControlPressed()){
				selectedKeySecondaryClick(args);
			}
		}
	}
	return selectedKeyframe != NULL;
}

void ofxTLKeyframes::regionSelected(ofLongRange timeRange, ofRange valueRange){
    for(int i = 0; i < keyframes.size(); i++){
        if(timeRange.contains(keyframes[i]->time) && valueRange.contains(1.-keyframes[i]->value)){
            selectKeyframe(keyframes[i]);
        }
	}
	updateKeyframeSort();
}

//update the grabTimeOffset to prepare for stretching keys
void ofxTLKeyframes::updateStretchOffsets(ofVec2f screenpoint, long grabMillis){
	for(int k = 0; k < selectedKeyframes.size(); k++){
        selectedKeyframes[k]->grabTimeOffset = selectedKeyframes[k]->time - stretchAnchor;
	}
}

void ofxTLKeyframes::updateDragOffsets(ofVec2f screenpoint, long grabMillis){
	for(int k = 0; k < selectedKeyframes.size(); k++){
        selectedKeyframes[k]->grabTimeOffset  = grabMillis - selectedKeyframes[k]->time;
        selectedKeyframes[k]->grabValueOffset = screenpoint.y - valueToScreenY(selectedKeyframes[k]->value);
	}
}

void ofxTLKeyframes::mouseMoved(ofMouseEventArgs& args, long millis){
	ofxTLTrack::mouseMoved(args, millis);
	hoverKeyframe = keyframeAtScreenpoint( ofVec2f(args.x, args.y));
}

void ofxTLKeyframes::mouseDragged(ofMouseEventArgs& args, long millis){

	if(keysAreStretchable){
		//cast the stretch anchor to long so that it can be signed
		float stretchRatio = 1.0*(millis-long(stretchAnchor)) / (1.0*stretchSelectPoint-stretchAnchor);

        for(int k = 0; k < selectedKeyframes.size(); k++){
            setKeyframeTime(selectedKeyframes[k], ofClamp(stretchAnchor + (selectedKeyframes[k]->grabTimeOffset * stretchRatio),
														  0, timeline->getDurationInMilliseconds()));
            selectedKeyframes[k]->screenPosition = screenPositionForKeyframe(selectedKeyframes[k]);
		}
        timeline->flagUserChangedValue();
        keysDidDrag = true;
        updateKeyframeSort();
	}

    if(keysAreDraggable && selectedKeyframes.size() != 0){
        ofVec2f screenpoint(args.x,args.y);
        for(int k = 0; k < selectedKeyframes.size(); k++){
            ofVec2f newScreenPosition;
            setKeyframeTime(selectedKeyframes[k], ofClamp(millis - selectedKeyframes[k]->grabTimeOffset,
														  screenXToMillis(bounds.getMinX()), screenXToMillis(bounds.getMaxX())));
            selectedKeyframes[k]->value = screenYToValue(args.y - selectedKeyframes[k]->grabValueOffset);
            selectedKeyframes[k]->screenPosition = screenPositionForKeyframe(selectedKeyframes[k]);
        }
        if(selectedKeyframe != NULL && timeline->getMovePlayheadOnDrag()){
            timeline->setCurrentTimeMillis(selectedKeyframe->time);
        }
        timeline->flagUserChangedValue();
        keysDidDrag = true;
        updateKeyframeSort();
    }
	createNewOnMouseup = false;
}

void ofxTLKeyframes::updateKeyframeSort(){
	//reset these caches because they may no longer be valid
	shouldRecomputePreviews = true;
	lastKeyframeIndex = 1;
	lastSampleTime = 0;
	if(keyframes.size() > 1){
		//modify duration to fit
		for(int i = 0; i < keyframes.size(); i++){
			if(keyframes[i]->time > timeline->getDurationInMilliseconds()){
				//keyframes[i]->time = timeline->getDurationInMilliseconds();
				timeline->setDurationInMillis(keyframes[i]->time);
			}
		}
		
		sort(keyframes.begin(), keyframes.end(), keyframesort);
		
		for(int i = 0; i < keyframes.size()-1; i++){
			if(keyframes[i]->time == keyframes[i+1]->time){
				if(keyframes[i]->previousTime < keyframes[i+1]->time){
					keyframes[i]->time -= 1;
				}
				else{
					keyframes[i+1]->time+=1;
				}
			}
		}
		if(selectedKeyframes.size() > 1){
			sort(selectedKeyframes.begin(), selectedKeyframes.end(), keyframesort);
		}
	}
}

void ofxTLKeyframes::mouseReleased(ofMouseEventArgs& args, long millis){
	keysAreDraggable = false;
    if(keysDidDrag){
		//reset these caches because they may no longer be valid
		lastKeyframeIndex = 1;
		lastSampleTime = 0;
        timeline->flagTrackModified(this);
    }
	
	if(createNewOnMouseup){
		//add a new one
		selectedKeyframe = newKeyframe();
		setKeyframeTime(selectedKeyframe,millis);
		selectedKeyframe->value = screenYToValue(args.y);
		keyframes.push_back(selectedKeyframe);
		selectedKeyframes.push_back(selectedKeyframe);
		updateKeyframeSort();
		timeline->flagTrackModified(this);
	}
	createNewOnMouseup = false;
}

void ofxTLKeyframes::setKeyframeTime(ofxTLKeyframe* key, unsigned long long newTime){
	key->previousTime = key->time;
	key->time = newTime;
}

void ofxTLKeyframes::getSnappingPoints(set<unsigned long long>& points){
	for(int i = 0; i < keyframes.size(); i++){
		if (isKeyframeIsInBounds(keyframes[i]) && !isKeyframeSelected(keyframes[i])) {
			points.insert(keyframes[i]->time);
		}
	}
}

vector<ofxTLKeyframe*>& ofxTLKeyframes::getKeyframes(){
    return keyframes;
}

string ofxTLKeyframes::copyRequest(){
	if(selectedKeyframes.size() > 0){
		return getXMLStringForKeyframes(selectedKeyframes);
	}
	return "";
}

string ofxTLKeyframes::cutRequest(){
	if(selectedKeyframes.size() > 0){
		string xmlrep = getXMLStringForKeyframes(selectedKeyframes);
		deleteSelectedKeyframes();
		return xmlrep;
	}
	return "";
}

void ofxTLKeyframes::pasteSent(string pasteboard){
	vector<ofxTLKeyframe*> keyContainer;
	ofxXmlSettings pastedKeys;
	
	if(pastedKeys.loadFromBuffer(pasteboard)){
		createKeyframesFromXML(pastedKeys, keyContainer);
		if(keyContainer.size() != 0){
			timeline->unselectAll();
			int numKeyframesPasted = 0;
			//normalize and add at playhead
			//for(int i = 0; i < keyContainer.size(); i++){
			for(int i = keyContainer.size()-1; i >= 0; i--){
				keyContainer[i]->time -= keyContainer[0]->time;
				keyContainer[i]->time += timeline->getCurrentTimeMillis();
				if(keyContainer[i]->time <= timeline->getDurationInMilliseconds()){
					selectedKeyframes.push_back(keyContainer[i]);
					keyframes.push_back(keyContainer[i]);
					numKeyframesPasted++;
				}
				else{
					delete keyContainer[i];
				}
			}

			if(numKeyframesPasted > 0){
				updateKeyframeSort();
				timeline->flagTrackModified(this);
			}
			
//			if(timeline->getMovePlayheadOnPaste()){
//				timeline->setCurrentTimeMillis( keyContainer[keyContainer.size()-1]->time );
//			}
		}
	}
}

void ofxTLKeyframes::addKeyframe(){
	addKeyframe(defaultValue);
}

void ofxTLKeyframes::addKeyframe(float value){
	//play solo change
	addKeyframeAtMillis(value, currentTrackTime());
}

void ofxTLKeyframes::addKeyframeAtMillis(unsigned long long millis){
	addKeyframeAtMillis(defaultValue, millis);
}

void ofxTLKeyframes::addKeyframeAtMillis(float value, unsigned long long millis){
	ofxTLKeyframe* key = newKeyframe();
	key->time = key->previousTime = millis;
	key->value = ofMap(value, valueRange.min, valueRange.max, 0, 1.0, true);
	keyframes.push_back(key);
	//smart sort, only sort if not added to end
	if(keyframes.size() > 2 && keyframes[keyframes.size()-2]->time > keyframes[keyframes.size()-1]->time){
		updateKeyframeSort();
	}
	lastKeyframeIndex = 1;
	timeline->flagTrackModified(this);
	shouldRecomputePreviews = true;
}

void ofxTLKeyframes::selectAll(){
	selectedKeyframes = keyframes;
}

void ofxTLKeyframes::unselectAll(){
	selectedKeyframes.clear();
}

int ofxTLKeyframes::getSelectedItemCount(){
    return selectedKeyframes.size();
}

unsigned long long ofxTLKeyframes::getEarliestTime(){
	if(keyframes.size() > 0){
		return keyframes[0]->time;
	}
	else{
		return ofxTLTrack::getEarliestTime();
	}
}

unsigned long long ofxTLKeyframes::getLatestTime(){
	if(keyframes.size() > 0){
		return keyframes[keyframes.size()-1]->time;
	}
	else{
		return ofxTLTrack::getLatestTime();
	}	
}

unsigned long long ofxTLKeyframes::getEarliestSelectedTime(){
	if(selectedKeyframes.size() > 0){
		return selectedKeyframes[0]->time;
	}
	else{
		return ofxTLTrack::getEarliestSelectedTime();
	}
}

unsigned long long ofxTLKeyframes::getLatestSelectedTime(){
	if(selectedKeyframes.size() > 0){
		return selectedKeyframes[selectedKeyframes.size()-1]->time;
	}
	else{
		return ofxTLTrack::getLatestSelectedTime();
	}	
}

string ofxTLKeyframes::getXMLRepresentation(){
    return getXMLStringForKeyframes(keyframes);
}

void ofxTLKeyframes::loadFromXMLRepresentation(string rep){
    clear();
    ofxXmlSettings buffer;
    buffer.loadFromBuffer(rep);
    createKeyframesFromXML(buffer, keyframes);
    updateKeyframeSort();
    timeline->flagUserChangedValue();    //because this is only called in Undo we don't flag track modified
}

//experimental binary saving. does not work with subclasses yet
void ofxTLKeyframes::saveToBinaryFile(){
	//write header
	//# keyframes (int),
	//size per keyframe (int)
	//keyframe chain
	string filePath = ofFilePath::removeExt(xmlFileName) + ".bin";
	
	int numKeys = keyframes.size();
	int keyBytes = sizeof(unsigned long long) + sizeof(float); //time + value
	cout << "saving binary file " << filePath << " with # keys " << numKeys << endl;
	ofFile outfile(ofToDataPath(filePath), ofFile::WriteOnly, true);
	outfile.write((char*)&numKeys,sizeof(int));
	outfile.write((char*)&keyBytes, sizeof(int));
	for(int i = 0; i < keyframes.size(); i++){
		outfile.write( (char*)&keyframes[i]->time, sizeof(unsigned long long));
		outfile.write( (char*)&keyframes[i]->value, sizeof(float));
	}
	outfile.close();
}

void ofxTLKeyframes::loadFromBinaryFile(){
	
	string filePath = ofFilePath::removeExt(xmlFileName) + ".bin";
	
	if(!ofFile(filePath).exists()){
		cout << "binary file doesn't exist " << filePath << endl;
		return;
	}
	cout << " found file " << filePath << endl;
	keyframes.clear();
	ofFile infile(ofToDataPath(filePath), ofFile::ReadOnly, true);
	int numKeys, keyBytes;
	infile.read( (char*)&numKeys, sizeof(int) );
	infile.read( (char*)&keyBytes, sizeof(int) );
	cout << "# keys " << numKeys << " of size " << keyBytes << endl;
	for(int i = 0; i < numKeys; i++){
		ofxTLKeyframe* k = newKeyframe();
		infile.read( (char*)&k->time, sizeof(unsigned long long) );
		infile.read( (char*)&k->value, sizeof(float) );
		keyframes.push_back(k);
	}
	shouldRecomputePreviews = true;
}

void ofxTLKeyframes::keyPressed(ofKeyEventArgs& args){
	if(args.key == OF_KEY_DEL || args.key == OF_KEY_BACKSPACE){
		deleteSelectedKeyframes();
	}
}

void ofxTLKeyframes::nudgeBy(ofVec2f nudgePercent){
	for(int i = 0; i < selectedKeyframes.size(); i++){
		setKeyframeTime(selectedKeyframes[i], ofClamp(selectedKeyframes[i]->time + timeline->getDurationInMilliseconds()*nudgePercent.x,
													  0, timeline->getDurationInMilliseconds()));
		selectedKeyframes[i]->value = ofClamp(selectedKeyframes[i]->value + nudgePercent.y, 0, 1.0);
	}	
	updateKeyframeSort();
    timeline->flagTrackModified(this);
}

void ofxTLKeyframes::deleteSelectedKeyframes(){
	//vector<ofxTLKeyframe*>::iterator selectedIt = selectedKeyframes.end();
	for(int i = keyframes.size() - 1; i >= 0; i--){
		if(isKeyframeSelected(keyframes[i])){
			if(keyframes[i] != selectedKeyframes[selectedKeyframes.size()-1]){
				ofLogError("ofxTLKeyframes::deleteSelectedKeyframes") << "keyframe delete inconsistency";
			}
			willDeleteKeyframe(keyframes[i]);
			if(keyframes[i] == hoverKeyframe){
				hoverKeyframe = NULL;
			}
			delete keyframes[i];
			keyframes.erase(keyframes.begin()+i);
			//if(selectedIt != selectedKeyframes.begin()){
//				selectedIt--;
				selectedKeyframes.erase(--selectedKeyframes.end());
			//}
		}
	}
	
	selectedKeyframes.clear();
	updateKeyframeSort();
    
    timeline->flagTrackModified(this);
}

void ofxTLKeyframes::deleteKeyframe(ofxTLKeyframe* keyframe){
	
	if(keyframe == NULL) return;
	
	for(int i = keyframes.size() - 1; i >= 0; i--){
		if(keyframe == keyframes[i]){
			deselectKeyframe(keyframe);
			willDeleteKeyframe(keyframes[i]);
			delete keyframes[i];
			keyframes.erase(keyframes.begin()+i);
			return;
		}
	}
}

ofxTLKeyframe* ofxTLKeyframes::keyframeAtScreenpoint(ofVec2f p){
	if(!bounds.inside(p)){
		return NULL;	
	}
	float minDistanceSquared = 15*15;
	for(int i = 0; i < keyframes.size(); i++){
		if(isKeyframeIsInBounds(keyframes[i]) &&
		   p.squareDistance(screenPositionForKeyframe(keyframes[i])) < minDistanceSquared)
		{
			return keyframes[i];
		}
	}
	return NULL;
}

void ofxTLKeyframes::selectKeyframe(ofxTLKeyframe* k){
	if(!isKeyframeSelected(k)){
        selectedKeyframes.push_back(k);
    }
}

void ofxTLKeyframes::deselectKeyframe(ofxTLKeyframe* k){
	for(int i = 0; i < selectedKeyframes.size(); i++){
        if(selectedKeyframes[i] == k){
            selectedKeyframes.erase(selectedKeyframes.begin() + i);
			return;
        }
    }
}

bool ofxTLKeyframes::isKeyframeSelected(ofxTLKeyframe* k){
	
	if(k == NULL) return false;

	return binary_search(selectedKeyframes.begin(), selectedKeyframes.end(), k, keyframesort);
//	for(int i = 0; i < selectedKeyframes.size(); i++){
//		if(selectedKeyframes[i] == k){
//			return true;
//		}
//	}
//	return false;
}

bool ofxTLKeyframes::isKeyframeIsInBounds(ofxTLKeyframe* key){
	if(zoomBounds.min == 0.0 && zoomBounds.max == 1.0) return true;
	unsigned long long duration = timeline->getDurationInMilliseconds();
	return key->time >= zoomBounds.min*duration && key->time <= zoomBounds.max*duration;
}

ofVec2f ofxTLKeyframes::screenPositionForKeyframe(ofxTLKeyframe* keyframe){
    return ofVec2f(millisToScreenX(keyframe->time), 
                   valueToScreenY(keyframe->value));
}

bool ofxTLKeyframes::screenpointIsInBounds(ofVec2f screenpoint){
	return bounds.inside(screenpoint);
}

float ofxTLKeyframes::screenYToValue(float screenY){
    return ofMap(screenY, bounds.y, bounds.y+bounds.height, 1.0, 0.0, true);
}

float ofxTLKeyframes::valueToScreenY(float value){
	return ofMap(value, 1.0, 0.0, bounds.y, bounds.y+bounds.height, true);
}

ofxTLKeyframe* ofxTLKeyframes::newKeyframe(){
	ofxTLKeyframe* k = new ofxTLKeyframe();
	return k;
}

string ofxTLKeyframes::getTrackType(){
    return "Keyframes";
}