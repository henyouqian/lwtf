#ifndef __LW_SOUND_H__
#define __LW_SOUND_H__

#include "lwtf/lwSingleton.h"
#include <list>

namespace lw{
	
    class SoundBufferRes;
	class Sound{
	public:
        Sound* create(const char *file, bool isPath);
		~Sound();
		int play(bool newSource);
        
		void stop(int source);
        void stopAll();
		void setLoop(bool b);
        bool isPlaying();
        bool isLoop();
		
		void setVolume(float volume);
        float getVolume();
		void setPitch(float pitch);
        float getPitch();
		void setOffset(float offsetSec);
		float getOffset();

	private:
		Sound(const char *file, bool isPath, bool &ok);
		bool _loop;
        std::list<int>_sources;
		float _volume;
		float _pitch;
		float _secOffset;
        SoundBufferRes* _pBuffRes;
    };
    
    void soundInit();
    void soundExit();
	

} //namespace lw

#endif //__LW_SOUND_H__