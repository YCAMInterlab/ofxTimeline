#pragma once

#include "ofConstants.h"

#include "ofMain.h"
#include "ofBaseSoundPlayer.h"
#include "ofEvents.h"
#include "ofThread.h"

#if defined (TARGET_OF_IPHONE) || defined (TARGET_OSX)
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#else
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include "kiss_fft.h"
#include "kiss_fftr.h"

#include <sndfile.h>

#ifdef OF_USING_MPG123
	#include <mpg123.h>
#endif

//		TO DO :
//		---------------------------
// 		-fft via fmod, as in the last time...
// 		-close fmod if it's up
//		-loadSoundForStreaming(char * fileName);
//		---------------------------

// 		interesting:
//		http://www.compuphase.com/mp3/mp3loops.htm


// ---------------------------------------------------------------------------- SOUND SYSTEM FMOD

// --------------------- global functions:
//void ofFmodSoundStopAll();
//void ofFmodSoundSetVolume(float vol);
//void ofOpenALSoundUpdate();						// calls FMOD update.
//float * ofFmodSoundGetSpectrum(int nBands);		// max 512...

//virtual void loadSound(string fileName, bool stream = false) = 0;
//virtual void unloadSound() = 0;
//virtual void play() = 0;
//virtual void stop() = 0;
//
//virtual void setVolume(float vol) = 0;
//virtual void setPan(float vol) = 0;
//virtual void setSpeed(float spd) = 0;
//virtual void setPaused(bool bP) = 0;
//virtual void setLoop(bool bLp) = 0;
//virtual void setMultiPlay(bool bMp) = 0;
//virtual void setPosition(float pct) = 0; // 0 = start, 1 = end;
//virtual void setPositionMS(int ms) = 0;
//
//virtual float getPosition() = 0;
//virtual int getPositionMS() = 0;
//virtual bool getIsPlaying() = 0;
//virtual float getSpeed() = 0;
//virtual float getPan() = 0;
//virtual bool isLoaded() = 0;
//virtual float getVolume() = 0;

// --------------------- player functions:
class ofOpenALSoundPlayer_TimelineAdditions : public ofBaseSoundPlayer, public ofThread {

	public:

		ofOpenALSoundPlayer_TimelineAdditions();
		virtual ~ofOpenALSoundPlayer_TimelineAdditions();

		bool loadSound(string fileName, bool stream = false);
		void unloadSound();
		void play();
		void stop();

		void setVolume(float vol);
		void setPan(float vol);
		void setSpeed(float spd);
		void setPaused(bool bP);
		void setLoop(bool bLp);
		void setMultiPlay(bool bMp);
		void setPosition(float pct); // 0 = start, 1 = end;
	    void setPositionMS(int ms);
    
		float getPosition();
	    int getPositionMS();
		bool getIsPlaying();
		float getSpeed();
		float getPan();
	    float getVolume();
	    bool isLoaded();
		bool getIsPaused();
		float getDuration();
		int getNumChannels();
    
		static void initialize();
		static void close();

        //averaging implementation
		vector<float>& getSpectrum(int bands);
        void setLogAverages(int minBandwidth, int bandsPerOctave);
        int getMinBandwidth();
        int getBandsPerOctave();
        vector<float>& getAverages();
        vector<short> & getBuffer();
        vector<float>& getCurrentBuffer(int _size);
        vector<float>& getBufferForFrame(int _frame, float _fps, int _size);
        vector<float> currentBuffer;

        float * getSystemSpectrum(int bands);

		static ALCcontext * alContext;
	protected:
		
        void threadedFunction();

		void ofOpenALSoundUpdate();
		void update(ofEventArgs & args);
		void initFFT(int bands);
		float *getCurrentBufferSum(int size);
    
		void createWindow(int size);
		void runWindow(vector<float> & signal);
		void initSystemFFT(int bands);

		bool sfReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
		bool sfStream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
#ifdef OF_USING_MPG123
		bool mpg123ReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
		bool mpg123Stream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);
#endif
        bool decoderReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer);

		void readFile(string fileName,vector<short> & buffer);
		void stream(string fileName, vector<short> & buffer);

		bool isStreaming;
		bool bMultiPlay;
		bool bLoop;
		bool bLoadedOk;
		bool bPaused;
		float pan; // 0 - 1
		float volume; // 0 - 1
		float internalFreq; // 44100 ?
		float speed; // -n to n, 1 = normal, -1 backwards
		unsigned int length; // in samples;

		static ALCdevice * alDevice;

	
		vector<float> window;
		float windowSum;
        float bandWidth;
    
		int channels;
		float duration; //in secs
		int samplerate;
		vector<ALuint> buffers;
		vector<ALuint> sources;

		// fft structures
		vector<vector<float> > fftBuffers;
		kiss_fftr_cfg fftCfg;
		vector<float> windowedSignal;
		vector<float> bins;
    	vector<float> averages;
        int currentMinBandwidth;
        int currentBandsPerOctave;

        int avgPerOctave;
        int octaves;
		vector<kiss_fft_cpx> cx_out;

        float calculateAverage(float lowFreq, float hiFreq);
        int freqToIndex(float freq);

		static kiss_fftr_cfg systemFftCfg;
		static vector<float> systemWindowedSignal;
		static vector<float> systemBins;
		static vector<kiss_fft_cpx> systemCx_out;

		SNDFILE* streamf;
		size_t stream_samples_read;
#ifdef OF_USING_MPG123
		mpg123_handle * mp3streamf;
#endif
		int stream_encoding;
		int mp3_buffer_size;
		int stream_subformat;
		double stream_scale;
		vector<short> buffer;
		vector<float> fftAuxBuffer;
        float curMaxAverage;
    
		bool stream_end;
    
        bool timeSet;
    float justSetTime;
};

