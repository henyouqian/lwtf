#include "stdafx.h"
#include "lwSound.h"
#include "lwFileSys.h"
#include "lwRes.h"
#include "lwLog.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include "ALUT/alut.h"
#include <map>


namespace lw{
    
    
    
    class SoundBufferRes : public Res{
    public:
        static SoundBufferRes* create(const char* file, bool isPath);
        ~SoundBufferRes();
        int getId();
    
    private:
        SoundBufferRes(const char* file, bool isPath, bool &ok);
        
        unsigned int _buffId;
        std::string _file;
    };
    
    namespace {
        std::map<std::string, SoundBufferRes*> _buffMap;
    }
    
    SoundBufferRes* SoundBufferRes::create(const char* file, bool isPath){
        std::map<std::string, SoundBufferRes*>::iterator it = _buffMap.find(file);
        if ( it != _buffMap.end() ){
            it->second->retain();
            return it->second;
        }else{
            bool ok = false;
            SoundBufferRes* p = new SoundBufferRes(file, isPath, ok);
            if ( p && ok ){
                _buffMap[file] = p;
                return p;
            }else if (p){
                delete p;
            }
        }
        return NULL;
    }
    
    SoundBufferRes::SoundBufferRes(const char* file, bool isPath, bool &ok){
        _file = file;
        if ( isPath ){
            _buffId = alutCreateBufferFromFile(file);
        }else{
            _buffId = alutCreateBufferFromFile(_f(file));
        }
        int e = alGetError();
        int ee = alutGetError();
        if ( e != AL_NO_ERROR || ee != AL_NO_ERROR ){
            lwerror("al error: " << e << " alut error:" << alutGetErrorString(ee));
            ok = false;
        }
        if ( _buffId == AL_NONE ){
            lwerror("alutCreateBufferFromFile failed");
            ok = false;
        }
        ok = true;
    }
    
    SoundBufferRes::~SoundBufferRes(){
        if ( _buffId != AL_NONE ){
            alDeleteBuffers(1, &_buffId);
            _buffMap.erase(_file);
        }
    }
    
    int SoundBufferRes::getId(){
        return _buffId;
    }
    
    //=========================================

	SoundSource* SoundSource::create(const char *file, int sourceNum, bool isPath){
        bool ok = false;
        SoundSource* pSnd = new SoundSource(file, sourceNum, isPath, ok);
        if ( pSnd && ok ){
            return pSnd;
        }else if ( pSnd ){
            delete pSnd;
        }
        return NULL;
    }
    
    SoundSource::SoundSource(const char *file, int sourceNum, bool isPath, bool &ok)
    :_sources(NULL), _sourceNum(0), _currSourceIdx(-1){
        _pBuffRes = SoundBufferRes::create(file, isPath);
        if ( !_pBuffRes ){
            lwerror("create buffer failed.");
            ok = false;
            return;
        }
        sourceNum = std::max(std::min(sourceNum, 5), 1);
        _sources = new unsigned int[sourceNum];
        alGenSources(sourceNum, _sources);
        int err = alGetError();
        if ( err != AL_NO_ERROR ){
            lwerror("alGenSources error:" << err );
            ok = false;
            return;
        }
        for ( int i = 0; i < sourceNum; ++i ){
            alSourcei(_sources[i], AL_BUFFER, _pBuffRes->getId());
            int err = alGetError();
            if ( err != AL_NO_ERROR ){
                lwerror("al bind buffer error:" << err );
                ok = false;
                return;
            }
        }
        
        _sourceNum = sourceNum;
        ok = true;
    }
    
    SoundSource::~SoundSource(){
        if ( _sourceNum ){
            for ( int i = 0; i < _sourceNum; ++i ){
                alSourcei(_sources[i], AL_BUFFER, 0);
            }
            alDeleteSources(_sourceNum, _sources);
            delete [] _sources;
        }
        if ( _pBuffRes ){
            _pBuffRes->release();
        }
    }
    
    void SoundSource::play(){
        ++_currSourceIdx;
        if ( _currSourceIdx == _sourceNum ){
            _currSourceIdx = 0;
        }
        alSourcePlay(_sources[_currSourceIdx]);
    }
    
    void SoundSource::stop(){
        alSourceStop(_sources[_currSourceIdx]);
    }
    
    bool SoundSource::isPlaying(){
        ALint state;
        if ( _currSourceIdx == -1 ){
            return false;
        }else{
            alGetSourcei(_sources[_currSourceIdx], AL_SOURCE_STATE, &state);
            return state == AL_PLAYING;
        }
    }
    
    void SoundSource::setLoop(bool b){
        _loop = b;
        for ( int i = 0; i < _sourceNum; ++i ){
            alSourcei(_sources[i], AL_LOOPING, _loop?1:0);
        }
    }
    
    bool SoundSource::isLoop(){
        return _loop;
    }
    
    void SoundSource::setVolume(float volume){
        _volume = volume;
        for ( int i = 0; i < _sourceNum; ++i ){
            alSourcef(_sources[_currSourceIdx], AL_GAIN, volume);
        }
    }
    
    float SoundSource::getVolume(){
        return _volume;
    }
    
    void SoundSource::setPitch(float pitch){
        _pitch = pitch;
        for ( int i = 0; i < _sourceNum; ++i ){
            alSourcef(_sources[_currSourceIdx], AL_PITCH, pitch);
        }
    }
    
    float SoundSource::getPitch(){
        return _pitch;
    }
    
    void SoundSource::setOffset(float secOffset){
        _secOffset = secOffset;
        for ( int i = 0; i < _sourceNum; ++i ){
            alSourcef(_sources[_currSourceIdx], AL_SEC_OFFSET, secOffset);
        }
    }
    
    float SoundSource::getOffset(){
        return _secOffset;
    }
    
    //==================================
        
    void soundInit(){
        alutInit (NULL, NULL);
    }
    
    void soundExit(){
        alutExit();
    }


} //namespace lw
