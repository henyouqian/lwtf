#include "stdafx.h"
#include "lwtf/lwTexture.h"
#include "lwtf/lwFileSys.h"
#include "lwtf/lwLog.h"
#include "soil/SOIL.h"

namespace lw{

    namespace{
        std::map<std::string, TextureRes*> _resMap;
    }
	

	TextureRes::TextureRes(const char* fileName, bool& ok) : _glId(-1){
        ok = false;
		lwassert(fileName);
		_fileName = fileName;

		size_t len = strlen(fileName);
		if ( len < 4 ){
			lwerror("texture file name too short: filepath = " << _f(fileName));
			_glId = -1;
			return;
		}
		_f fpath(fileName);
        if ( fpath.isValid() ){
            loadAndCreateOgl(fpath);
        }else{
            lwerror("texture file is not exist: " << fileName);
            return;
        }
        ok = true;
	}

	void TextureRes::loadAndCreateOgl(const char* path){
		unsigned char* pImgData = SOIL_load_image(path, &_w, &_h, &_numChannels, SOIL_LOAD_AUTO);
		_glId = SOIL_internal_create_OGL_texture(pImgData, _w, _h, _numChannels,
			SOIL_CREATE_NEW_ID, 0,
			GL_TEXTURE_2D, GL_TEXTURE_2D,
			GL_MAX_TEXTURE_SIZE);
		
		SOIL_free_image_data(pImgData);
		
		if ( _glId == 0 ){
			lwerror("Failed to load texture: path=" << path);
			_glId = -1;
		}
	}

	TextureRes::~TextureRes(){
		if ( _glId != -1 ){
			glDeleteTextures(1, &_glId);
			if ( !_fileName.empty() ){
                _resMap.erase(_fileName);
			}
		}
	}
	TextureRes* TextureRes::create(const char* fileName){
		lwassert(fileName);
		std::string strFileName = fileName;
		std::map<std::string, TextureRes*>::iterator it = _resMap.find(strFileName);
		if ( it == _resMap.end() ){
            bool ok = false;
			TextureRes* p = new TextureRes(fileName, ok);
			if ( p && !ok  ){
				delete p;
				return NULL;
			}else{
				_resMap[strFileName] = p;
				return p;
			}
		}else{
			it->second->retain();
			return it->second;
		}
	}

	void TextureRes::clean(){
		while( !_resMap.empty() ){
			lwerror("Texture resource leak: file=" << _resMap.begin()->first.c_str());
			delete (_resMap.begin()->second);
		}
		_resMap.clear();
	}
    
    GLuint TextureRes::getGlId(){
        return _glId;
    }
    
    const char* TextureRes::getFileName(){
        return _fileName.c_str();
    }
    
    int TextureRes::getWidth(){
        return _w;
    }
    
    int TextureRes::getHeight(){
        return _h;
    }
    
    int TextureRes::getChannelNum(){
        return _numChannels;
    }

} //namespace lw