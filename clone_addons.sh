#!/bin/bash

# Dependencies for ofxTimeline

cd ../

if [ -z $1 ]; then
    PREFIX="git clone https://github.com/"
else
    PREFIX="git clone git@github.com:"
fi

${PREFIX}YCAMInterlab/ofxTimecode.git

${PREFIX}obviousjim/ofxTween.git

${PREFIX}obviousjim/ofxMSATimer.git

${PREFIX}Flightphase/ofxTextInputField.git

${PREFIX}Flightphase/ofxRange.git
