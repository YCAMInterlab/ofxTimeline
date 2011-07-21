/*
 *  ofxTimelineUtils.h
 *  timelineExample
 *
 *  Created by Jim on 7/19/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

static bool isPointInRect(ofVec2f p, ofRectangle r){
	return p.x > r.x && p.y > r.y && p.x < r.x+r.width && p.y < r.y+r.height;
}

