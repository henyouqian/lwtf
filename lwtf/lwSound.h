#ifndef __LW_Sound_H__
#define __LW_Sound_H__

#include "lwtf/lwSingleton.h"

namespace lw{
	
    class SoundBufferRes;
	class SoundSource{
	public:
        static SoundSource* create(const char *file, int sourceNum, bool isPath);
		~SoundSource();
		void play();
		void stop();
		void setLoop(bool b);
        bool isPlaying();
        bool isLoop();
		
		void setVolume(float volume);
        float getVolume();
		void setPitch(float pitch);
        float getPitch();
		void setOffset(float secOffset);
		float getOffset();

	private:
		SoundSource(const char *file, int sourceNum, bool isPath, bool &ok);
        unsigned int *_sources;
        int _sourceNum;
        int _currSourceIdx;
		bool _loop;
		float _volume;
		float _pitch;
		float _secOffset;
        SoundBufferRes* _pBuffRes;
    };
    
    void soundInit();
    void soundExit();
	
    class AudioPlayer{
    public:
        static AudioPlayer* create(const char *file);
        ~AudioPlayer();
        void play();
        void pause();
        void stop();
        void setCurrentTime(float second);
        void setNumberOfLoops(int loops);
        void setVolume(float volume);
        float getVolume();
        bool isPlaying();
        
    private:
        AudioPlayer(const char *file, bool &ok);
        struct Data;
        Data* _pData;
    };
    

} //namespace lw

#endif //__LW_Sound_H__