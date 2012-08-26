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
    
    
    //=========================================

	Sound* Sound::create(const char *file, bool isPath){
        bool ok = false;
        Sound* pSnd = new Sound(file, isPath, ok);
        if ( pSnd && ok ){
            return pSnd;
        }else if ( pSnd ){
            delete pSnd;
        }
        return NULL;
    }
    
    Sound::Sound(const char *file, bool isPath, bool &ok){
        _pBuffRes = SoundBufferRes::create(file, isPath);
        if ( !_pBuffRes ){
            lwerror("create buffer failed.");
            ok = false;
            return;
        }
        
        ok = true;
    }
    
    Sound::~Sound(){
        if ( _pBuffRes ){
            _pBuffRes->release();
        }
    }
    
    int Sound::play(){
        
    }
    
    void Sound::stop(int source){
        
    }
    
    void Sound::stopAll(){
        
    }
    
    void Sound::setLoop(bool b){
        _loop = b;
    }
    
    bool Sound::isPlaying(){
        return !_sources.empty();
    }
    
    bool Sound::isLoop(){
        return _loop;
    }
    
    void Sound::setVolume(float volume){
        _volume = volume;
        
    }
    
    float Sound::getVolume(){
        return _volume;
    }
    
    void Sound::setPitch(float pitch){
        _pitch = pitch;
        if ( !_sources.empty() ){
            alSourcef(_sources.back(), AL_PITCH, pitch);
            
        }
    }
    
    float Sound::getPitch(){
        return _pitch;
    }
    
    void Sound::setOffset(float offsetSec){
        _secOffset = offsetSec;
    }
    
    float Sound::getOffset(){
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
