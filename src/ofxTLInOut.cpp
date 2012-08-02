
#include "ofxTLInOut.h"
#include "ofxTimeline.h"

ofxTLInOut::ofxTLInOut()
 : 	dragOffset(0), 
	draggingIn(false), 
    draggingOut(false),
	hoveringIn(false),
	hoveringOut(false)
{
    
}

void ofxTLInOut::draw(){
    ofPushStyle();
    ofSetLineWidth(3);
    int inScreenX = normalizedXtoScreenX( timeline->getInOutRange().min );
    int outScreenX = normalizedXtoScreenX( timeline->getInOutRange().max ); 
    
    if(inScreenX > bounds.x && inScreenX < bounds.x+bounds.width){
        if(hoveringIn){
            ofSetColor(timeline->getColors().highlightColor);
        }
        else{
	        ofSetColor(timeline->getColors().keyColor);
        }
        ofLine(inScreenX, bounds.y, inScreenX, bounds.y+bounds.height);
    }

    if(outScreenX > bounds.x && outScreenX < bounds.x+bounds.width){
        if(hoveringIn){
            ofSetColor(timeline->getColors().highlightColor);
        }
        else{
	        ofSetColor(timeline->getColors().keyColor);
        }
        ofLine(outScreenX, bounds.y, outScreenX, bounds.y+bounds.height);
    }
    
    //draw inout over the whole thing
    ofFill();
    ofSetLineWidth(1);
	//draw in/out point
	float inPointX = normalizedXtoScreenX(timeline->getInOutRange().min);
	float outPointX = normalizedXtoScreenX(timeline->getInOutRange().max);
    
	if(bounds.x < inPointX){
		ofSetColor(timeline->getColors().disabledColor,120);
		ofRect(bounds.x, pageRect.y, inPointX - bounds.x, pageRect.height);
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(inPointX, pageRect.y, inPointX, pageRect.y+pageRect.height);
	}
	
	if(bounds.x+bounds.width > outPointX){
		ofSetColor(timeline->getColors().disabledColor,120);
		ofRect(outPointX, bounds.y, (bounds.x+bounds.width) - outPointX, pageRect.height);	
		ofSetColor(timeline->getColors().highlightColor);
		ofLine(outPointX, bounds.y, outPointX, bounds.y+pageRect.height);
	}
	
	ofPopStyle();

}

void ofxTLInOut::setPageRectangle(ofRectangle pageRectangle){
    pageRect = pageRectangle;
}

void ofxTLInOut::mouseMoved(ofMouseEventArgs& args){

    if(!bounds.inside(args.x,args.y)) return;
    
    hoveringIn  = abs( normalizedXtoScreenX( timeline->getInOutRange().min ) - args.x) < 7;
	hoveringOut = abs( normalizedXtoScreenX( timeline->getInOutRange().max ) - args.x) < 7;
}

void ofxTLInOut::mousePressed(ofMouseEventArgs& args){
    
    if(!bounds.inside(args.x,args.y)) return;
       
    
	float distToIn = normalizedXtoScreenX( timeline->getInOutRange().min ) - args.x;
    if(abs(distToIn) < 7){
        draggingIn = true;
        dragOffset = distToIn;
    }
    
    float distToOut = normalizedXtoScreenX( timeline->getInOutRange().max ) - args.x;
    if(!draggingIn && abs(distToOut) < 7){
        draggingOut = true;
        dragOffset = distToOut;
    }

    if(!draggingIn && !draggingOut){
        if(abs(distToOut) > abs(distToIn)){
            draggingIn = true;
            dragOffset = 0;
            timeline->setInPointAtPercent(screenXtoNormalizedX(args.x));            
        }
        else{
            draggingOut = true;
            dragOffset = 0;
            timeline->setOutPointAtPercent(screenXtoNormalizedX(args.x));
        }
    }
    
    cout << "dist to in " << abs(distToIn) << " out " << abs(distToOut) << " " << draggingIn << " " << draggingOut << endl;
}

void ofxTLInOut::mouseDragged(ofMouseEventArgs& args){
    if(draggingIn){
        timeline->setInPointAtPercent(screenXtoNormalizedX(args.x - dragOffset));
    }
    if(draggingOut){
        timeline->setOutPointAtPercent(screenXtoNormalizedX(args.x - dragOffset));
    }
}

void ofxTLInOut::mouseReleased(ofMouseEventArgs& args){
    draggingIn = draggingOut = false;
}

