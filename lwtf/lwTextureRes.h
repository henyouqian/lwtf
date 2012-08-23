#ifndef __LW_TEXTURE_H__
#define __LW_TEXTURE_H__

#include "lwRes.h"
#include "OpenGLES/ES2/gl.h"
#include <map>

namespace lw{

	class TextureRes : public Res{
	public:
		static TextureRes* create(const char* fileName, bool reserveData = false, bool loadOnly = false, bool revertY = false);	//reserveData only for PNG
		static TextureRes* create(int w, int h, const char* pData, bool hasAlpha, bool reserveData, bool revertY);
		GLuint getGlId() const{
			return _glId;
		}
		const char* getFileName(){return _fileName.c_str();}
		const unsigned char* getData() const{
			return _pImgData;
		}
		int getWidth(){
			return _w;
		}
		int getHeight(){
			return _h;
		}
		int getChannelNum(){
			return _numChannels;
		}
		void reserveData();
        void createOgl();

	private:
		~TextureRes();
		TextureRes(const char* filename, bool& ok, bool reserveData = false, bool loadOnly = false, bool revertY = false);
		TextureRes(int w, int h, const char* pData, bool hasAlpha, bool reserveData = false, bool revertY = false);
		void loadAndCreateOgl(const char* path, bool reserveData, bool revertY);
        void loadImage(const char* path, bool reserveData, bool revertY);

	private:
		GLuint _glId;
		std::string _fileName;
		int _numChannels;
		int _w, _h;
		unsigned char* _pImgData;

		static std::map<std::string, TextureRes*> _resMap;
        bool _reserveData;
        bool _revertY;
		
	public:
		static void clean();
	};

} //namespace lw


#endif //__LW_TEXTURE_H__