#include "ofOpenALSoundPlayer_TimelineAdditions.h"

//#ifdef OF_SOUND_PLAYER_OPENAL

#include "ofUtils.h"
#include "ofMath.h"
#include "ofFileUtils.h"
#include "ofAppRunner.h"
#include <set>
#ifdef TARGET_WIN32
#include <math.h>
#endif

#include "ofxAudioDecoder.h"

ALCdevice * ofOpenALSoundPlayer_TimelineAdditions::alDevice = 0;
ALCcontext * ofOpenALSoundPlayer_TimelineAdditions::alContext = 0;
//vector<float> ofOpenALSoundPlayer_TimelineAdditions::window;
//float ofOpenALSoundPlayer_TimelineAdditions::windowSum=0;


kiss_fftr_cfg ofOpenALSoundPlayer_TimelineAdditions::systemFftCfg=0;
vector<float> ofOpenALSoundPlayer_TimelineAdditions::systemWindowedSignal;
vector<float> ofOpenALSoundPlayer_TimelineAdditions::systemBins;
vector<kiss_fft_cpx> ofOpenALSoundPlayer_TimelineAdditions::systemCx_out;

static set<ofOpenALSoundPlayer_TimelineAdditions*> players;

void ofOpenALSoundUpdate_TimelineAdditions(){
	alcProcessContext(ofOpenALSoundPlayer_TimelineAdditions::alContext);
}

#define BUFFER_STREAM_SIZE 4096

// now, the individual sound player:
//------------------------------------------------------------
ofOpenALSoundPlayer_TimelineAdditions::ofOpenALSoundPlayer_TimelineAdditions(){
	bLoop 			= false;
	bLoadedOk 		= false;
	pan 			= 0.5f;
	volume 			= 1.0f;
	internalFreq 	= 44100;
	speed 			= 1;
	bPaused 		= false;
	isStreaming		= false;
	channels		= 0;
	duration		= 0;
	fftCfg			= 0;
	streamf			= 0;
    octaves         = 0;
    curMaxAverage   = 0;
    timeSet         = false;
#ifdef OF_USING_MPG123
	mp3streamf		= 0;
#endif
	players.insert(this);
}

// ----------------------------------------------------------------------------
ofOpenALSoundPlayer_TimelineAdditions::~ofOpenALSoundPlayer_TimelineAdditions(){
	unloadSound();
	kiss_fftr_free(fftCfg);
	players.erase(this);
}

//---------------------------------------
// this should only be called once
void ofOpenALSoundPlayer_TimelineAdditions::initialize(){
	if(!alDevice){
		alDevice = alcOpenDevice(NULL);
		alContext = alcCreateContext(alDevice,NULL);
		alcMakeContextCurrent (alContext);
		alListener3f(AL_POSITION, 0,0,0);
#ifdef OF_USING_MPG123
		mpg123_init();
#endif

	}
}

//---------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::createWindow(int size){
	if(int(window.size())!=size){
		windowSum = 0;
		window.resize(size);
        
        bandWidth = (2.0f / size) * (samplerate / 2.0f);
		// hanning window

		for(int i = 0; i < size; i++){
			window[i] = .54 - .46 * cos((TWO_PI * i) / (size - 1));
			windowSum += window[i];
		}
	}
}

//---------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::close(){
	if(alDevice){
		alcCloseDevice(alDevice);
		alDevice = NULL;
		alcDestroyContext(alContext);
		alContext = 0;
#ifdef OF_USING_MPG123
		mpg123_exit();
#endif
	}
}

// ----------------------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::sfReadFile(string path, vector<short> & buffer, vector<float> & fftAuxBuffer){
	SF_INFO sfInfo;
	SNDFILE* f = sf_open(path.c_str(),SFM_READ,&sfInfo);
	if(!f){
		ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read " + path);
		return false;
	}

	buffer.resize(sfInfo.frames*sfInfo.channels);
	fftAuxBuffer.resize(sfInfo.frames*sfInfo.channels);

	int subformat = sfInfo.format & SF_FORMAT_SUBMASK ;
	if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE){
		double	scale ;
		sf_command (f, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
		if (scale < 1e-10)
			scale = 1.0 ;
		else
			scale = 32700.0 / scale ;

		sf_count_t samples_read = sf_read_float (f, &fftAuxBuffer[0], fftAuxBuffer.size());
		if(samples_read<(int)fftAuxBuffer.size())
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read " + path);
		for (int i = 0 ; i < int(fftAuxBuffer.size()) ; i++){
			fftAuxBuffer[i] *= scale ;
			buffer[i] = 32565.0 * fftAuxBuffer[i];
		}
	}else{
		sf_count_t frames_read = sf_readf_short(f,&buffer[0],sfInfo.frames);
		if(frames_read<sfInfo.frames){
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read buffer for " + path);
			return false;
		}
		sf_seek(f,0,SEEK_SET);
		frames_read = sf_readf_float(f,&fftAuxBuffer[0],sfInfo.frames);
		if(frames_read<sfInfo.frames){
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read fft buffer for " + path);
			return false;
		}
	}
	sf_close(f);

	channels = sfInfo.channels;
	duration = float(sfInfo.frames) / float(sfInfo.samplerate);
	samplerate = sfInfo.samplerate;
	return true;
}

#ifdef OF_USING_MPG123
//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::mpg123ReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
	int err = MPG123_OK;
	mpg123_handle * f = mpg123_new(NULL,&err);
	if(mpg123_open(f,path.c_str())!=MPG123_OK){
		ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read " + path);
		return false;
	}

	int encoding;
	long int rate;
	mpg123_getformat(f,&rate,&channels,&encoding);
	if(encoding!=MPG123_ENC_SIGNED_16){
		ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: unsupported encoding");
		return false;
	}
	samplerate = rate;

	size_t done=0;
	size_t buffer_size = mpg123_outblock( f );
	buffer.resize(buffer_size/2);
	while(mpg123_read(f,(unsigned char*)&buffer[buffer.size()-buffer_size/2],buffer_size,&done)!=MPG123_DONE){
		buffer.resize(buffer.size()+buffer_size/2);
	};
	buffer.resize(buffer.size()-(buffer_size/2-done/2));
	mpg123_close(f);
	mpg123_delete(f);

	fftAuxBuffer.resize(buffer.size());
	for(int i=0;i<(int)buffer.size();i++){
		fftAuxBuffer[i] = float(buffer[i])/32565.f;
	}
	duration = float(buffer.size()/channels) / float(samplerate);
	return true;
}
#endif

//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::decoderReadFile(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
    
    ofxAudioDecoder audioDecoder;
    audioDecoder.load(path);
    
    if (audioDecoder.getNumSamples() == 0) {
        ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read " + path);
        return false;
    }
    
    buffer.resize(audioDecoder.getNumFrames() * audioDecoder.getChannels());
    fftAuxBuffer.resize(audioDecoder.getNumFrames() * audioDecoder.getChannels());
    
    memcpy(fftAuxBuffer.data(), audioDecoder.getRawSamples().data(), audioDecoder.getNumSamples() * sizeof(float));
    
    for (int i = 0; i < fftAuxBuffer.size(); ++i) {
        buffer[i] = 32565.0 * fftAuxBuffer[i];
    }
    
    channels = audioDecoder.getChannels();
    duration = float(audioDecoder.getNumFrames()) / float(audioDecoder.getSampleRate());
    samplerate = audioDecoder.getSampleRate();
    return true;
}

//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::sfStream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
	if(!streamf){
		SF_INFO sfInfo;
		streamf = sf_open(path.c_str(),SFM_READ,&sfInfo);
		if(!streamf){
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read " + path);
			return false;
		}

		stream_subformat = sfInfo.format & SF_FORMAT_SUBMASK ;
		if (stream_subformat == SF_FORMAT_FLOAT || stream_subformat == SF_FORMAT_DOUBLE){
			sf_command (streamf, SFC_CALC_SIGNAL_MAX, &stream_scale, sizeof (stream_scale)) ;
			if (stream_scale < 1e-10)
				stream_scale = 1.0 ;
			else
				stream_scale = 32700.0 / stream_scale ;
		}
		channels = sfInfo.channels;
		duration = float(sfInfo.frames) / float(sfInfo.samplerate);
		samplerate = sfInfo.samplerate;
		stream_samples_read = 0;
	}

	int curr_buffer_size = BUFFER_STREAM_SIZE*channels;
	if(speed>1) curr_buffer_size *= (int)floor(speed+.5);
	buffer.resize(curr_buffer_size);
	fftAuxBuffer.resize(buffer.size());
	if (stream_subformat == SF_FORMAT_FLOAT || stream_subformat == SF_FORMAT_DOUBLE){
		sf_count_t samples_read = sf_read_float (streamf, &fftAuxBuffer[0], fftAuxBuffer.size());
		stream_samples_read += samples_read;
		if(samples_read<(int)fftAuxBuffer.size()){
			fftAuxBuffer.resize(samples_read);
			buffer.resize(samples_read);
			setPosition(0);
			if(!bLoop) stopThread();
			stream_samples_read = 0;
			stream_end = true;
		}
		for (int i = 0 ; i < int(fftAuxBuffer.size()) ; i++){
			fftAuxBuffer[i] *= stream_scale ;
			buffer[i] = 32565.0 * fftAuxBuffer[i];
		}
	}else{
		sf_count_t frames_read = sf_readf_short(streamf,&buffer[0],curr_buffer_size/channels);
		stream_samples_read += frames_read*channels;
		if(frames_read<curr_buffer_size/channels){
			fftAuxBuffer.resize(frames_read*channels);
			buffer.resize(frames_read*channels);
			setPosition(0);
			if(!bLoop) stopThread();
			stream_samples_read = 0;
			stream_end = true;
		}
		for(int i=0;i<(int)buffer.size();i++){
			fftAuxBuffer[i]=float(buffer[i])/32565.0f;
		}
	}

	return true;
}

#ifdef OF_USING_MPG123
//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::mpg123Stream(string path,vector<short> & buffer,vector<float> & fftAuxBuffer){
	if(!mp3streamf){
		int err = MPG123_OK;
		mp3streamf = mpg123_new(NULL,&err);
		if(mpg123_open(mp3streamf,path.c_str())!=MPG123_OK){
			mpg123_close(mp3streamf);
			mpg123_delete(mp3streamf);
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: couldnt read " + path);
			return false;
		}

		long int rate;
		mpg123_getformat(mp3streamf,&rate,&channels,&stream_encoding);
		if(stream_encoding!=MPG123_ENC_SIGNED_16){
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: unsupported encoding");
			return false;
		}
		samplerate = rate;
		mp3_buffer_size = mpg123_outblock( mp3streamf );


		mpg123_seek(mp3streamf,0,SEEK_END);
		off_t samples = mpg123_tell(mp3streamf);
		duration = float(samples/channels) / float(samplerate);
		mpg123_seek(mp3streamf,0,SEEK_SET);
	}

	int curr_buffer_size = mp3_buffer_size;
	if(speed>1) curr_buffer_size *= (int)round(speed);
	buffer.resize(curr_buffer_size);
	fftAuxBuffer.resize(buffer.size());
	size_t done=0;
	if(mpg123_read(mp3streamf,(unsigned char*)&buffer[0],curr_buffer_size*2,&done)==MPG123_DONE){
		setPosition(0);
		buffer.resize(done/2);
		fftAuxBuffer.resize(done/2);
		if(!bLoop) stopThread();
		stream_end = true;
	}


	for(int i=0;i<(int)buffer.size();i++){
		fftAuxBuffer[i] = float(buffer[i])/32565.f;
	}

	return true;
}
#endif

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::stream(string fileName, vector<short> & buffer){
#ifdef OF_USING_MPG123
	if(ofFilePath::getFileExt(fileName)=="mp3" || ofFilePath::getFileExt(fileName)=="MP3" || mp3streamf){
		if(!mpg123Stream(fileName,buffer,fftAuxBuffer)) return;
	}else
#endif
		if(!sfStream(fileName,buffer,fftAuxBuffer)) return;

	fftBuffers.resize(channels);
	int numFrames = buffer.size()/channels;

	for(int i=0;i<channels;i++){
		fftBuffers[i].resize(numFrames);
		for(int j=0;j<numFrames;j++){
			fftBuffers[i][j] = fftAuxBuffer[j*channels+i];
		}
	}
}

void ofOpenALSoundPlayer_TimelineAdditions::readFile(string fileName, vector<short> & buffer){
    if(ofFilePath::getFileExt(fileName)!="mp3" && ofFilePath::getFileExt(fileName)!="MP3"){
		if(!sfReadFile(fileName,buffer,fftAuxBuffer)) return;
	}else{
#ifdef OF_USING_MPG123
		if(!mpg123ReadFile(fileName,buffer,fftAuxBuffer)) return;
#else
        if(!decoderReadFile(fileName,buffer,fftAuxBuffer)) return;
#endif
    }

    fftBuffers.resize(channels);
	int numFrames = buffer.size()/channels;

	for(int i=0;i<channels;i++){
		fftBuffers[i].resize(numFrames);
		for(int j=0;j<numFrames;j++){
			fftBuffers[i][j] = fftAuxBuffer[j*channels+i];
		}
	}
}

//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::loadSound(string fileName, bool is_stream){

    string ext = ofToLower(ofFilePath::getFileExt(fileName));
    if(ext != "wav" && ext != "aif" && ext != "aiff" && ext != "mp3"){
        ofLogError("Sound player can only load .wav .aiff or .mp3 files");
        return false;
    }
       
    fileName = ofToDataPath(fileName);

	bLoadedOk = false;
	bMultiPlay = false;
	isStreaming = is_stream;
	
	// [1] init sound systems, if necessary
	initialize();

	// [2] try to unload any previously loaded sounds
	// & prevent user-created memory leaks
	// if they call "loadSound" repeatedly, for example

	unloadSound();

	ALenum format=AL_FORMAT_MONO16;

    if(!isStreaming || ext == "mp3"){ // mp3s don't stream cause they gotta be decoded
		readFile(fileName, buffer);
	}else{
		stream(fileName, buffer);
	}

    if(channels == 0){
        ofLogError("ofOpenALSoundPlayer_TimelineAdditions -- File not found");
        return false;
    }
    
	int numFrames = buffer.size()/channels;
	if(isStreaming){
		buffers.resize(channels*2);
	}else{
		buffers.resize(channels);
	}
	alGenBuffers(buffers.size(), &buffers[0]);
	if(channels==1){
		sources.resize(1);
		alGenSources(1, &sources[0]);
		if (alGetError() != AL_NO_ERROR){
			ofLog(OF_LOG_WARNING,"ofOpenALSoundPlayer_TimelineAdditions: openAL error reported generating sources for " + fileName);
			return false;
		}

		for(int i=0; i<(int)buffers.size(); i++){
			alBufferData(buffers[i],format,&buffer[0],buffer.size()*2,samplerate);
			if (alGetError() != AL_NO_ERROR){
				ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: error creating buffer");
				return false;
			}
			if(isStreaming){
				stream(fileName,buffer);
			}
		}
		if(isStreaming){
			alSourceQueueBuffers(sources[0],buffers.size(),&buffers[0]);
		}else{
			alSourcei (sources[0], AL_BUFFER,   buffers[0]);
		}

		alSourcef (sources[0], AL_PITCH,    1.0f);
		alSourcef (sources[0], AL_GAIN,     1.0f);
	    alSourcef (sources[0], AL_ROLLOFF_FACTOR,  0.0);
	    alSourcei (sources[0], AL_SOURCE_RELATIVE, AL_TRUE);
	}else{
		vector<vector<short> > multibuffer;
		multibuffer.resize(channels);
		sources.resize(channels);
		alGenSources(channels, &sources[0]);
		if(isStreaming){
			for(int s=0; s<2;s++){
				for(int i=0;i<channels;i++){
					multibuffer[i].resize(buffer.size()/channels);
					for(int j=0;j<numFrames;j++){
						multibuffer[i][j] = buffer[j*channels+i];
					}
					alBufferData(buffers[s*2+i],format,&multibuffer[i][0],buffer.size()/channels*2,samplerate);
					if (alGetError() != AL_NO_ERROR){
						ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: error creating stereo buffers for " + fileName);
						return false;
					}
					alSourceQueueBuffers(sources[i],1,&buffers[s*2+i]);
					stream(fileName,buffer);
				}
			}
		}else{
			for(int i=0;i<channels;i++){
				multibuffer[i].resize(buffer.size()/channels);
				for(int j=0;j<numFrames;j++){
					multibuffer[i][j] = buffer[j*channels+i];
				}
				alBufferData(buffers[i],format,&multibuffer[i][0],buffer.size()/channels*2,samplerate);
				if (alGetError() != AL_NO_ERROR){
					ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: error creating stereo buffers for " + fileName);
					return false;
				}
				alSourcei (sources[i], AL_BUFFER,   buffers[i]   );
			}
		}

		for(int i=0;i<channels;i++){
			if (alGetError() != AL_NO_ERROR){
				ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: error creating stereo sources for " + fileName);
				return false;
			}

			// only stereo panning
			if(i==0){
				float pos[3] = {-1,0,0};
				alSourcefv(sources[i],AL_POSITION,pos);
			}else{
				float pos[3] = {1,0,0};
				alSourcefv(sources[i],AL_POSITION,pos);
			}
			alSourcef (sources[i], AL_ROLLOFF_FACTOR,  0.0);
			alSourcei (sources[i], AL_SOURCE_RELATIVE, AL_TRUE);
		}
	}
	ofLogVerbose("ofOpenALSoundPlayer_TimelineAdditions: successfully loaded " + fileName);
	bLoadedOk = true;
	return true;
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::threadedFunction(){
	vector<vector<short> > multibuffer;
	multibuffer.resize(channels);
	while(isThreadRunning()){
        cout << "threaded function" << endl;
		for(int i=0; i<int(sources.size())/channels; i++){
			int processed;
			alGetSourcei(sources[i*channels], AL_BUFFERS_PROCESSED, &processed);

			while(processed--)
			{
				stream("",buffer);
				int numFrames = buffer.size()/channels;
				if(channels>1){
					for(int j=0;j<channels;j++){
						multibuffer[j].resize(buffer.size()/channels);
						for(int k=0;k<numFrames;k++){
							multibuffer[j][k] = buffer[k*channels+j];
						}
						ALuint albuffer;
						alSourceUnqueueBuffers(sources[i*channels+j], 1, &albuffer);
						alBufferData(albuffer,AL_FORMAT_MONO16,&multibuffer[j][0],buffer.size()*2/channels,samplerate);
						alSourceQueueBuffers(sources[i*channels+j], 1, &albuffer);
					}
				}else{
					ALuint albuffer;
					alSourceUnqueueBuffers(sources[i], 1, &albuffer);
					alBufferData(albuffer,AL_FORMAT_MONO16,&buffer[0],buffer.size()*2/channels,samplerate);
					alSourceQueueBuffers(sources[i], 1, &albuffer);
				}
				if(stream_end){
					break;
				}
			}
			ALint state;
			alGetSourcei(sources[i*channels],AL_SOURCE_STATE,&state);
			bool stream_running=false;
			#ifdef OF_USING_MPG123
				stream_running = streamf || mp3streamf;
			#else
				stream_running = streamf;
			#endif
			if(state != AL_PLAYING && stream_running && !stream_end){
				alSourcePlayv(channels,&sources[i*channels]);
			}

			stream_end = false;
		}
        timeSet = false;

		ofSleepMillis(1);
	}
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::update(ofEventArgs & args){
    if(bMultiPlay){
        for(int i=1; i<int(sources.size())/channels; ){
            ALint state;
            alGetSourcei(sources[i*channels],AL_SOURCE_STATE,&state);
            if(state != AL_PLAYING){
                alDeleteSources(channels,&sources[i*channels]);
                for(int j=0;j<channels;j++){
                    sources.erase(sources.begin()+i*channels);
                }
            }else{
                i++;
            }
        }
    }
    timeSet = false;
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::unloadSound(){
    
//	ofRemoveListener(ofEvents.update,this,&ofOpenALSoundPlayer_TimelineAdditions::update);
	if(isLoaded()){
        ofRemoveListener(ofEvents().update,this,&ofOpenALSoundPlayer_TimelineAdditions::update);

		alDeleteBuffers(buffers.size(),&buffers[0]);
		alDeleteSources(sources.size(),&sources[0]);
        
        bLoadedOk = false;
	}
	streamf = 0;
}

//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::getIsPlaying(){
	if(sources.empty()) return false;
	if(isStreaming) return isThreadRunning();
	ALint state;
	bool playing=false;
	for(int i=0;i<(int)sources.size();i++){
		alGetSourcei(sources[i],AL_SOURCE_STATE,&state);
		playing |= (state == AL_PLAYING);
	}
	return playing;
}

//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::getIsPaused(){
	if(sources.empty()) return false;
	ALint state;
	bool paused=true;
	for(int i=0;i<(int)sources.size();i++){
		alGetSourcei(sources[i],AL_SOURCE_STATE,&state);
		paused &= (state == AL_PAUSED);
	}
	return paused;
}

//------------------------------------------------------------
float ofOpenALSoundPlayer_TimelineAdditions::getSpeed(){
	return speed;
}

//------------------------------------------------------------
float ofOpenALSoundPlayer_TimelineAdditions::getPan(){
	return pan;
}

float ofOpenALSoundPlayer_TimelineAdditions::getDuration(){
	return duration;
}

int ofOpenALSoundPlayer_TimelineAdditions::getNumChannels(){
	return channels;
}

//------------------------------------------------------------
vector<short> & ofOpenALSoundPlayer_TimelineAdditions::getBuffer(){
	return buffer;
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setVolume(float vol){
	volume = vol;
	if(sources.empty()) return;
	if(channels==1){
		alSourcef (sources[sources.size()-1], AL_GAIN, vol);
	}else{
		setPan(pan);
	}
}

//------------------------------------------------------------
float ofOpenALSoundPlayer_TimelineAdditions::getVolume(){
	return volume;    
}

//------------------------------------------------------------
bool ofOpenALSoundPlayer_TimelineAdditions::isLoaded(){
    return bLoadedOk;
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setPosition(float pct){
	if(sources.empty()) return;
#ifdef OF_USING_MPG123
	if(mp3streamf){
		mpg123_seek(mp3streamf,duration*pct*samplerate*channels,SEEK_SET);
	}else
#endif
	if(streamf){
		sf_seek(streamf,duration*pct*samplerate*channels,SEEK_SET);
		stream_samples_read = 0;
	}else{
		for(int i=0;i<(int)channels;i++){
			alSourcef(sources[sources.size()-channels+i],AL_SEC_OFFSET,pct*duration);
		}
	}
    timeSet = true;
    justSetTime = pct;
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setPositionMS(int ms){
    if(duration == 0) return;

	setPosition( ms / 1000. / duration);
}

//------------------------------------------------------------
float ofOpenALSoundPlayer_TimelineAdditions::getPosition(){
	if(duration==0) return 0;
	if(sources.empty()) return 0;
	float pos;
#ifdef OF_USING_MPG123
	if(mp3streamf){
		pos = float(mpg123_tell(mp3streamf)) / float(channels) / float(samplerate);
	}else
#endif
	if(streamf){
		pos = float(stream_samples_read) / float(channels) / float(samplerate);
		return pos/duration;
	}else{
        if(timeSet) return justSetTime;
		alGetSourcef(sources[sources.size()-1],AL_SAMPLE_OFFSET,&pos);
        return channels*(pos/buffer.size());
        
        //alGetSourcef(sources[sources.size()-1],AL_SEC_OFFSET,&pos);
        //return pos / duration;
	}
}

//------------------------------------------------------------
int ofOpenALSoundPlayer_TimelineAdditions::getPositionMS(){
	if(duration==0) return 0;
	if(sources.empty()) return 0;
	int pos;
#ifdef OF_USING_MPG123
	if(mp3streamf){
		pos = 1000 * float(mpg123_tell(mp3streamf)) / float(channels) / float(samplerate);
	}else
#endif
    if(streamf){
        pos = float(stream_samples_read) / float(channels) / float(samplerate);
        return pos * 1000;
    }else{
        float sampleOffset;
        alGetSourcef(sources[sources.size()-1],AL_SAMPLE_OFFSET,&sampleOffset);
        return 1000 * duration * channels * (sampleOffset/buffer.size());
    }
    return pos;
}

//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setPan(float p){
	if(sources.empty()) return;
	if(channels==1){
		p=p*2-1;
		float pos[3] = {p,0,0};
		alSourcefv(sources[sources.size()-1],AL_POSITION,pos);
	}else{
		for(int i=0;i<(int)channels;i++){
			if(i==0){
				alSourcef(sources[sources.size()-channels+i],AL_GAIN,(1-p)*volume);
			}else{
				alSourcef(sources[sources.size()-channels+i],AL_GAIN,p*volume);
			}
		}
	}
	pan = p;
}


//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setPaused(bool bP){
	if(sources.empty()) return;
	if(bP){
		alSourcePausev(sources.size(),&sources[0]);
	}else{
		alSourcePlayv(sources.size(),&sources[0]);
	}

	bPaused = bP;
}


//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setSpeed(float spd){
	for(int i=0;i<channels;i++){
		alSourcef(sources[sources.size()-channels+i],AL_PITCH,spd);
	}
	speed = spd;
}


//------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setLoop(bool bLp){
	if(bMultiPlay) return; // no looping on multiplay
	bLoop = bLp;
	if(isStreaming) return;
	for(int i=0;i<(int)sources.size();i++){
		alSourcei(sources[i],AL_LOOPING,bLp?AL_TRUE:AL_FALSE);
	}
}

// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setMultiPlay(bool bMp){
	if(isStreaming && bMp){
		ofLog(OF_LOG_WARNING,"ofOpenALSoundPlayer_TimelineAdditions: no support for multiplay streams by now");
		return;
	}
	bMultiPlay = bMp;		// be careful with this...
	if(sources.empty()) return;
//	if(bMultiPlay){
		ofAddListener(ofEvents().update,this,&ofOpenALSoundPlayer_TimelineAdditions::update);
//	}else{
//		ofRemoveListener(ofEvents().update,this,&ofOpenALSoundPlayer_TimelineAdditions::update);
//	}
}

// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::play(){

	// if the sound is set to multiplay, then create new sources,
	// do not multiplay on loop or we won't be able to stop it
	if (bMultiPlay && !bLoop){
		sources.resize(sources.size()+channels);
		alGenSources(channels, &sources[sources.size()-channels]);
		if (alGetError() != AL_NO_ERROR){
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: error creating multiplay stereo sources");
			return;
		}
		for(int i=0;i<channels;i++){
			alSourcei (sources[sources.size()-channels+i], AL_BUFFER,   buffers[i]   );
			// only stereo panning
			if(i==0){
				float pos[3] = {-1,0,0};
				alSourcefv(sources[sources.size()-channels+i],AL_POSITION,pos);
			}else{
				float pos[3] = {1,0,0};
				alSourcefv(sources[sources.size()-channels+i],AL_POSITION,pos);
			}
		    alSourcef (sources[sources.size()-channels+i], AL_ROLLOFF_FACTOR,  0.0);
		    alSourcei (sources[sources.size()-channels+i], AL_SOURCE_RELATIVE, AL_TRUE);
		}

		if (alGetError() != AL_NO_ERROR){
			ofLog(OF_LOG_ERROR,"ofOpenALSoundPlayer_TimelineAdditions: error assigning multiplay buffers");
			return;
		}
	}
	alSourcePlayv(channels,&sources[sources.size()-channels]);

//	if(bMultiPlay){
		ofAddListener(ofEvents().update,this,&ofOpenALSoundPlayer_TimelineAdditions::update);
//	}
	if(isStreaming){
		setPosition(0);
		stream_end = false;
		startThread(true,false);
	}

}

// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::stop(){
	alSourceStopv(channels,&sources[sources.size()-channels]);
}

// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::initFFT(int bands){
	if(int(bins.size())==bands) return;
	int signalSize = (bands-1)*2;
	if(fftCfg!=0) kiss_fftr_free(fftCfg);
	fftCfg = kiss_fftr_alloc(signalSize, 0, NULL, NULL);
	cx_out.resize(bands);
	bins.resize(bands);
	createWindow(signalSize);
}

// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::initSystemFFT(int bands){
	if(int(systemBins.size())==bands) return;
	int signalSize = (bands-1)*2;
	if(systemFftCfg!=0) kiss_fftr_free(systemFftCfg);
	systemFftCfg = kiss_fftr_alloc(signalSize, 0, NULL, NULL);
	systemCx_out.resize(bands);
	systemBins.resize(bands);
	createWindow(signalSize);
}

float * ofOpenALSoundPlayer_TimelineAdditions::getCurrentBufferSum(int size){
	if(int(windowedSignal.size())!=size){
		windowedSignal.resize(size);
	}
	windowedSignal.assign(windowedSignal.size(),0);
	for(int k=0;k<int(sources.size())/channels;k++){
		if(!isStreaming){
			ALint state;
			alGetSourcei(sources[k*channels],AL_SOURCE_STATE,&state);
			if( state != AL_PLAYING ) continue;
		}
		int pos;
		alGetSourcei(sources[k*channels],AL_SAMPLE_OFFSET,&pos);
		//if(pos+size>=(int)fftBuffers[0].size()) continue;
		for(int i=0;i<channels;i++){
			float gain;
			alGetSourcef(sources[k*channels+i],AL_GAIN,&gain);
			for(int j=0;j<size;j++){
				if(pos+j<(int)fftBuffers[i].size())
					windowedSignal[j]+=fftBuffers[i][pos+j]*gain;
				else
					windowedSignal[j]=0;
			}
		}
	}
	return &windowedSignal[0];
}

vector<float>& ofOpenALSoundPlayer_TimelineAdditions::getCurrentBuffer(int _size)
{
	if(int(currentBuffer.size()) != _size)
    {
        currentBuffer.resize(_size);
	}
   	currentBuffer.assign(currentBuffer.size(),0);

    int pos;
	for(int k = 0; k < int(sources.size())/channels; ++k)
    {
		alGetSourcei(sources[k*channels],AL_SAMPLE_OFFSET,&pos);
        for(int i = 0; i < channels; ++i)
        {
            for(int j = 0; j < _size; ++j)
            {
                if(pos+j<(int)buffer.size())
                {
                    currentBuffer[j] += float(buffer[(pos+j)*channels+i])/65534.0f;
                }
                else
                {
                    currentBuffer[j] = 0;
                }
            }
        }
    }
    return currentBuffer;
}

vector<float>& ofOpenALSoundPlayer_TimelineAdditions::getBufferForFrame(int _frame, float _fps, int _size)
{
	if(int(currentBuffer.size()) != _size)
    {
        currentBuffer.resize(_size);
	}
   	currentBuffer.assign(currentBuffer.size(),0);
    
    int pos = _frame*float(samplerate)/_fps;
	for(int k = 0; k < int(sources.size())/channels; ++k)
    {
        for(int i = 0; i < channels; ++i)
        {
            for(int j = 0; j < _size; ++j)
            {
                if(pos+j<(int)buffer.size())
                {
                    currentBuffer[j] += float(buffer[(pos+j)*channels+i])/65534.0f;
                }
                else
                {
                    currentBuffer[j] = 0;
                }
            }
        }
    }
    return currentBuffer;
}

// ----------------------------------------------------------------------------
vector<float>& ofOpenALSoundPlayer_TimelineAdditions::getSpectrum(int bands){

	initFFT(bands);
	bins.assign(bins.size(),0);
	if(sources.empty()) return bins;

	int signalSize = (bands-1)*2;
	getCurrentBufferSum(signalSize);

	float normalizer = 2. / windowSum;
	runWindow(windowedSignal);
	kiss_fftr(fftCfg, &windowedSignal[0], &cx_out[0]);
	for(int i= 0; i < bands; i++) {
		bins[i] += sqrtf(cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i) * normalizer;
	}
	return bins;
}

// ----------------------------------------------------------------------------
vector<float>& ofOpenALSoundPlayer_TimelineAdditions::getAverages(){
    
    if(averages.size() > 0){
        getSpectrum(bins.size());
        float max = 0;
        for (int i = 0; i < octaves; i++){
            float lowFreq, hiFreq, freqStep;
            if (i == 0){
                lowFreq = 0;
            }
            else{
                lowFreq = (samplerate / 2) / powf(2, octaves - i);
            }
            
            hiFreq = (samplerate / 2) / powf(2, octaves - i - 1);
            freqStep = (hiFreq - lowFreq) / avgPerOctave;
            float f = lowFreq;
            for (int j = 0; j < avgPerOctave; j++){
                int offset = j + i * avgPerOctave;
                averages[offset] = calculateAverage(f, f + freqStep);
                max = MAX(max,averages[offset]);
                f += freqStep;
            }
        }
    }
    
    
    return averages;
}

float ofOpenALSoundPlayer_TimelineAdditions::calculateAverage(float lowFreq, float hiFreq) {
    int lowBound = freqToIndex(lowFreq);
    int hiBound = freqToIndex(hiFreq);
    float avg = 0;
    for (int i = lowBound; i <= hiBound; i++) {
        avg += bins[i];
    }
    avg /= (hiBound - lowBound + 1);
    return avg;
}

int ofOpenALSoundPlayer_TimelineAdditions::freqToIndex(float freq) {
    if (freq < bandWidth / 2) return 0;
    if (freq > samplerate / 2 - window.size() / 2) return bins.size() - 1;
    float fraction = freq / samplerate;
    return int( floor(window.size() * fraction + .5) );
}

//http://code.compartmental.net/2007/03/21/fft-averages/
// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::setLogAverages(int minBandwidth, int bandsPerOctave){
    if(!isLoaded()){
        return;
    }
    
    currentMinBandwidth = minBandwidth;
    currentBandsPerOctave = bandsPerOctave;

    float nyquist = (float) samplerate / 2.0f;
    octaves = 1;
    while ((nyquist /= 2) > minBandwidth){
        octaves++;
    }
    
    avgPerOctave = bandsPerOctave;
    averages.resize(octaves * bandsPerOctave);
}

// ----------------------------------------------------------------------------
int ofOpenALSoundPlayer_TimelineAdditions::getMinBandwidth(){
    return currentMinBandwidth;
}

// ----------------------------------------------------------------------------
int ofOpenALSoundPlayer_TimelineAdditions::getBandsPerOctave(){
    return currentBandsPerOctave;
}

// ----------------------------------------------------------------------------
float * ofOpenALSoundPlayer_TimelineAdditions::getSystemSpectrum(int bands){
	initSystemFFT(bands);
	systemBins.assign(systemBins.size(),0);
	if(players.empty()) return &systemBins[0];

	int signalSize = (bands-1)*2;
	if(int(systemWindowedSignal.size())!=signalSize){
		systemWindowedSignal.resize(signalSize);
	}
	systemWindowedSignal.assign(systemWindowedSignal.size(),0);

	set<ofOpenALSoundPlayer_TimelineAdditions*>::iterator it;
	for(it=players.begin();it!=players.end();it++){
		if(!(*it)->getIsPlaying()) continue;
		float * buffer = (*it)->getCurrentBufferSum(signalSize);
		for(int i=0;i<signalSize;i++){
			systemWindowedSignal[i]+=buffer[i];
		}
	}

	float normalizer = 2. / windowSum;
	runWindow(systemWindowedSignal);
	kiss_fftr(systemFftCfg, &systemWindowedSignal[0], &systemCx_out[0]);
	for(int i= 0; i < bands; i++) {
		systemBins[i] += sqrtf(systemCx_out[i].r * systemCx_out[i].r + systemCx_out[i].i * systemCx_out[i].i) * normalizer;
	}
	return &systemBins[0];
}

// ----------------------------------------------------------------------------
void ofOpenALSoundPlayer_TimelineAdditions::runWindow(vector<float> & signal){
	for(int i = 0; i < (int)signal.size(); i++)
		signal[i] *= window[i];
}

//#endif
