
#pragma once
#include "ofMain.h"
#include "ofxTLTrack.h"

class ofxTLInOut : public ofxTLTrack {
  public:
    ofxTLInOut();
    
    virtual void draw();
	virtual void mousePressed(ofMouseEventArgs& args);
	virtual void mouseMoved(ofMouseEventArgs& args);
	virtual void mouseDragged(ofMouseEventArgs& args);
	virtual void mouseReleased(ofMouseEventArgs& args);
    
    void setPageRectangle(ofRectangle pageRectangle);
    
  protected:
    float dragOffset;
    //if both are set then it's a selection range
    bool draggingIn;
    bool draggingOut;
    bool hoveringIn;
    bool hoveringOut;
	    
    ofRectangle pageRect;
//    bool onInHandle(int screenX);
//    bool onOutHandle(int screenX);
};