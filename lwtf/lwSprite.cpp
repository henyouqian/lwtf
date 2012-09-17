#include "stdafx.h"
#include "lwSprite.h"
#include "lwTexture.h"
#include "lwShader.h"
#include "lwApp.h"
#include "lwFileSys.h"
#include "tinyxml2/tinyxml2.h"
#include <map>

namespace lw {
    
    namespace {
        struct AtlasInfo{
            std::string file;
            int u, v, w, h;
        };
        std::map<std::string, AtlasInfo> _atlasMap;
    }
    
    class SpriteVertexBuffer{
    public:
        SpriteVertexBuffer();
        ~SpriteVertexBuffer();
        void collectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId);
        void flush();
        
        enum{
            VERTICIS_NUM_RESERVE = 8192,
            VERTICIS_NUM_LIMIT = 65536,
        };
        
    private:
        std::vector<SpriteVertex> _vertices;
        Color _currColor;
        BlendMode _currBlendMode;
        GLuint _currTextureId;
        ShaderProgramRes* _pShaderProg;
        int _posLocation;
        int _uvLocation;
        int _mvpMatLocation;
        int _colorLocation;
        int _samplerLocation;
    };
    
    SpriteVertexBuffer::SpriteVertexBuffer(){
        _vertices.reserve(VERTICIS_NUM_RESERVE);
        std::vector<std::string> attrNames;
        attrNames.push_back("position");
        attrNames.push_back("uv");
        _pShaderProg = ShaderProgramRes::create("sprite.vsh", "sprite.fsh", attrNames);
        _posLocation = _pShaderProg->getAttribLocation("a_position");
        _uvLocation = _pShaderProg->getAttribLocation("a_uv");
        _mvpMatLocation = _pShaderProg->getUniformLocation("u_mvpmat");
        _colorLocation = _pShaderProg->getUniformLocation("u_color");
        _samplerLocation = _pShaderProg->getUniformLocation("u_texture");
    }
    
    SpriteVertexBuffer::~SpriteVertexBuffer(){
        if ( _pShaderProg ){
            _pShaderProg->release();
        }
    }
    
    void SpriteVertexBuffer::collectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId){
        if ( !_vertices.empty() && 
            ( color != _currColor 
             || blendMode != _currBlendMode 
             || textureId != _currTextureId 
             || _vertices.size() >= VERTICIS_NUM_LIMIT-3 ) )
        {
            flush();
        }
        _currColor = color;
        _currBlendMode = blendMode;
        _currTextureId = textureId;
        
        for ( int i = 0; i < numVertices; ++i ) {
            _vertices.push_back(vertices[i]);
        }
    }
    
    void SpriteVertexBuffer::flush(){
        if ( _vertices.empty() ){
            return;
        }
        cml::Matrix4 m;
		float w, h;
        App::s().getViewSize(w, h);
		cml::matrix_orthographic_RH(m, 0.f, w, -h, 0.f, -1000.f, 1000.f, cml::z_clip_neg_one);
        glUniformMatrix4fv(_mvpMatLocation, 1, false, m.data());
        glUniform4f(_colorLocation, _currColor.r/255.f, _currColor.g/255.f, _currColor.b/255.f, _currColor.a/255.f);
        
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _currTextureId);
        //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		//glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glUniform1i(_samplerLocation, 0);
        glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
        if ( _currBlendMode == BLEND_NONE ){
			glDisable(GL_BLEND);
		}else{
			glEnable(GL_BLEND);
			if ( _currBlendMode == BLEND_NORMAL ){
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}else if ( _currBlendMode == BLEND_ADD ){
				glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			}else if ( _currBlendMode == BLEND_MUL ){
                glBlendFunc(GL_ZERO, GL_SRC_COLOR);
            }
		}
        
        const char* p = (char*)&_vertices[0];
        glEnableVertexAttribArray(_posLocation);
        glVertexAttribPointer(_posLocation, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), p);
        glEnableVertexAttribArray(_uvLocation);
        glVertexAttribPointer(_uvLocation, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), p+3*sizeof(float));
        
        _pShaderProg->use();
        
        glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
        
        _vertices.clear();
    }
    
    namespace {
        SpriteVertexBuffer* _pvb = NULL;
    }
    
    //====================================================
    
	Sprite* Sprite::createFromFile(const char *file){
        bool ok = false;
        Sprite* p = new Sprite(file, false, ok);
        if ( p ){
            if ( ok ){
                return p;
            }else{
                delete p;
                return NULL;
            }
        }
        return NULL;
    }
    
    Sprite* Sprite::createFromAtlas(const char* key){
        bool ok = false;
        Sprite* p = new Sprite(key, true, ok);
        if ( p ){
            if ( ok ){
                return p;
            }else{
                delete p;
                return NULL;
            }
        }
        return NULL;
        return NULL;
    }
    
    namespace{
        void makeFileName2x(std::string& name){
            int n = name.find('.');
            std::string ext = &name[n];
            name.resize(n);
            name.append("_2x");
            name.append(ext);
        }
    }
    
    Sprite::Sprite(const char *file, bool fromAtlas, bool &ok){
        ok = false;
        _ancX = _ancY = 0.f;
        _posX = _posY = 0.f;
        _rotate = 0.f;
        _scaleX = _scaleY = 1.f;
        _color.set(255, 255, 255, 255);
        _blendMode = BLEND_NORMAL;
        _needUpdate = true;
        _pTextureRes = NULL;
        
        _is2x = false;
        bool already2x = false;
        std::string sfile = file;
        int pos = sfile.find("_2x.");
        if ( pos != std::string::npos ){
            already2x = true;
        }
        
        if ( lw::App::s().getScreenScale() == 2.f ){
            _is2x = true;
            if ( !already2x ){
                makeFileName2x(sfile);
            }
            if ( fromAtlas ){
                loadFromAtlas(sfile.c_str());
            }else{
                loadFromFile(sfile.c_str());
            }
        }
        if ( !_pTextureRes ){
            _is2x = false;
            if ( !already2x ){
                if ( fromAtlas ){
                    loadFromAtlas(file);
                }else{
                    loadFromFile(file);
                }
            }
        }
        
        if ( _pTextureRes ){
            ok = true;
        }
    }
    
    Sprite::~Sprite(){
        if ( _pTextureRes ){
			_pTextureRes->release();
		}
    }
    
    void Sprite::loadFromFile(const char *file){
        _pTextureRes = TextureRes::create(file);
        if ( _pTextureRes ){
            uvInit();
        }
    }
    
    void Sprite::loadFromAtlas(const char *key){
        std::map<std::string, AtlasInfo>::iterator it = _atlasMap.find(key);
        if ( it != _atlasMap.end() ){
            AtlasInfo& atlas = it->second;
            _pTextureRes = TextureRes::create(key);
            if ( _pTextureRes ){
                setUV(atlas.u, atlas.v, atlas.w, atlas.h);
            }
        }
    }
    
    void Sprite::setUV(float u, float v, float w, float h){
        float texW = (float)_pTextureRes->getWidth();
        float texH = (float)_pTextureRes->getHeight();
        
        if ( _is2x ){
            texW *= .5f;
            texH *= .5f;
        }
        
        _u = u;
        _v = v;
        _w = w;
        _h = h;
        
        _u1 = u/texW;
		_v1 = v/texH;
        _u2 = (u+_w)/texW;
		_v2 = (v+_h)/texH;
    }
    
    void Sprite::uvInit(){
        _u = _v = 0.f;
        _w = (float)_pTextureRes->getWidth();
        _h = (float)_pTextureRes->getHeight();
        if ( _is2x ){
            _w *= .5f;
            _h *= .5f;
        }
        _u1 = _v1 = 0.f;
        _u2 = _v2 = 1.f;
    }
    
    void Sprite::getUV(float &u, float &v, float &w, float &h){
        u = _u; v = _v; w = _w; h = _h;
    }
    
    void Sprite::setAnchor(float x, float y){
        if ( x != _ancX || y != _ancY ){
            _ancX = x; _ancY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getAnchor(float &x, float &y){
        x = _ancX; y = _ancY;
    }
    
    void Sprite::setPos(float x, float y){
        if ( x != _posX || y != _posY ){
            _posX = x;
            _posY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getPos(float &x, float &y){
        x = _posX;
        y = _posY;
    }
    
    void Sprite::setRotate(float r){
        if ( _rotate != r ){
            _rotate = r;
            _needUpdate = true;
        }
    }
    
    float Sprite::getRotate(){
        return _rotate;
    }
    
    void Sprite::setScale(float x, float y){
        if ( _scaleX != x || _scaleY != y ){
            _scaleX = x;
            _scaleY = y;
            _needUpdate = true;
        }
    }
    
    void Sprite::getScale(float &x, float &y){
        x = _scaleX; y = _scaleY;
    }
    
    void Sprite::setSize(float w, float h){
        setScale(w/_w, h/_h);
    }
    
    void Sprite::getSize(float &w, float &h){
        w = _w*_scaleX;
        h = _h*_scaleY;
    }
    
    void Sprite::setColor(const Color& color){
        _color = color;
    }
    
    void Sprite::setBlendMode(BlendMode blendMode){
        _blendMode = blendMode;
    }
    
    void Sprite::setMatrix(cml::Matrix3 m){
        cml::Matrix3 m1;
		cml::matrix_translation_2D(m, _posX, _posY);
		if ( _rotate != 0.f ){
			cml::matrix_rotation_2D(m1, _rotate);
			m *= m1;
		}
        float scaleX = _scaleX;
        float scaleY = _scaleY;
        if ( _is2x ){
            scaleX *= .5f;
            scaleY *= .5f;
        }
		if ( scaleX != 1.f || scaleY != 1.f ){
            cml::matrix_scale_2D(m1, scaleX, scaleY);
			m *= m1;
		}
        
        float posX1 = -_ancX;
		float posY1 = -_ancY;
		float posX2 = posX1 + _w;
		float posY2 = posY1 + _h;
        
		cml::Vector2 p1(posX1, posY1);
		p1 = cml::transform_point_2D(m, p1);
		cml::Vector2 p2(posX1, posY2);
		p2 = cml::transform_point_2D(m, p2);
		cml::Vector2 p3(posX2, posY1);
		p3 = cml::transform_point_2D(m, p3);
		cml::Vector2 p4(posX2, posY2);
		p4 = cml::transform_point_2D(m, p4);
        
        _vertexPos[0][0] = p1[0];
		_vertexPos[0][1] = -p1[1];
        
        _vertexPos[1][0] = p2[0];
		_vertexPos[1][1] = -p2[1];
        
        _vertexPos[2][0] = p3[0];
		_vertexPos[2][1] = -p3[1];
        
        _vertexPos[3][0] = p4[0];
		_vertexPos[3][1] = -p4[1];
    }
    
    void Sprite::draw(){
        if ( _needUpdate ){
            update();
        }
        //1
		SpriteVertex v[6];
		v[0].x = _vertexPos[0][0];
		v[0].y = _vertexPos[0][1];
		v[0].z = 0.f;
		v[0].u = _u1;
		v[0].v = _v1;
        
		//2
		v[1].x = _vertexPos[1][0];
		v[1].y = _vertexPos[1][1];
		v[1].z = 0.f;
		v[1].u = _u1;
		v[1].v = _v2;
        
		//3
		v[2].x = _vertexPos[2][0];
		v[2].y = _vertexPos[2][1];
		v[2].z = 0.f;
		v[2].u = _u2;
		v[2].v = _v1;
		
        //3
		v[3].x = _vertexPos[2][0];
		v[3].y = _vertexPos[2][1];
		v[3].z = 0.f;
		v[3].u = _u2;
		v[3].v = _v1;
		
		//2
		v[4].x = _vertexPos[1][0];
		v[4].y = _vertexPos[1][1];
		v[4].z = 0.f;
		v[4].u = _u1;
		v[4].v = _v2;
	    
		//4
		v[5].x = _vertexPos[3][0];
		v[5].y = _vertexPos[3][1];
		v[5].z = 0.f;
		v[5].u = _u2;
		v[5].v = _v2;
        
        _pvb->collectVetices(v, 6, _color, _blendMode, _pTextureRes->getGlId());
    }
    
    void Sprite::update(){
        _needUpdate = false;
        cml::Matrix3 m, m1;
		cml::matrix_translation_2D(m, _posX, _posY);
		if ( _rotate != 0.f ){
			cml::matrix_rotation_2D(m1, _rotate);
			m *= m1;
		}
        float scaleX = _scaleX;
        float scaleY = _scaleY;
//        if ( _is2x ){
//            scaleX *= .5f;
//            scaleY *= .5f;
//        }
		if ( scaleX != 1.f || scaleY != 1.f ){
            cml::matrix_scale_2D(m1, scaleX, scaleY);
			m *= m1;
		}
        
        float posX1 = -_ancX;
		float posY1 = -_ancY;
		float posX2 = posX1 + _w;
		float posY2 = posY1 + _h;
        
		cml::Vector2 p1(posX1, posY1);
		p1 = cml::transform_point_2D(m, p1);
		cml::Vector2 p2(posX1, posY2);
		p2 = cml::transform_point_2D(m, p2);
		cml::Vector2 p3(posX2, posY1);
		p3 = cml::transform_point_2D(m, p3);
		cml::Vector2 p4(posX2, posY2);
		p4 = cml::transform_point_2D(m, p4);
        
        _vertexPos[0][0] = p1[0];
		_vertexPos[0][1] = -p1[1];
        
        _vertexPos[1][0] = p2[0];
		_vertexPos[1][1] = -p2[1];
        
        _vertexPos[2][0] = p3[0];
		_vertexPos[2][1] = -p3[1];
        
        _vertexPos[3][0] = p4[0];
		_vertexPos[3][1] = -p4[1];
    }
    
    bool Sprite::is2x(){
        return _is2x;
    }
    
    //===============================================
    Sprite9* Sprite9::create(const char* file, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3){
		lwassert(file);
		bool ok = false;
		Sprite9* pSprite9 = new Sprite9(file, uvX, uvY, uvW1, uvW2, uvW3, uvH1, uvH2, uvH3, ok);
		lwassert(pSprite9);
		if ( !ok ){
			delete pSprite9;
			return NULL;
		}
		return pSprite9;
	}
	Sprite9::Sprite9(const char* file, int uvX, int uvY, int uvW1, int uvW2, int uvW3, int uvH1, int uvH2, int uvH3, bool& ok):_needUpdate(true), _is2x(false){
        
		_x[0] = 0.f;
		_y[0] = 0.f;
        
		_uvW[0] = uvW1;
		_uvW[1] = uvW2;
		_uvW[2] = uvW3;
        
		_uvH[0] = uvH1;
		_uvH[1] = uvH2;
		_uvH[2] = uvH3;
        
		_uvX[0] = uvX;
		_uvX[1] = _uvX[0] + uvW1;
		_uvX[2] = _uvX[1] + uvW2;
        
		_uvY[0] = uvY;
		_uvY[1] = _uvY[0] + uvH1;
		_uvY[2] = _uvY[1] + uvH2;
        
		_width = (float)(_uvW[0] + _uvW[1] + _uvW[2]);
		_height = (float)(_uvH[0] + _uvH[1] + _uvH[2]);
        
        memset(_pSprites, 0, sizeof(Sprite*)*9);
        for ( int i = 0; i < 9; ++i ){
            _pSprites[i] = Sprite::createFromFile(file);
            if ( _pSprites[i] == NULL ){
                lwerror("Sprite::create failed: file=" <<file);
                return;
            }
            _pSprites[i]->setUV(_uvX[i/3], _uvY[i%3], _uvW[i/3], _uvH[i%3]);
        }
        _is2x = _pSprites[0]->is2x();
        
		ok = true;
	}
	Sprite9::~Sprite9(){
		for ( int i = 0; i < 9; ++i ){
            if ( _pSprites[i] )
                delete _pSprites[i];
		}
	}
	void Sprite9::setPos(float x, float y){
		_x[0] = x;
		_y[0] = y;
		_needUpdate = true;
	}
	void Sprite9::setSize(float w, float h){
		_width = w;
		_height = h;
		_needUpdate = true;
	}
	void Sprite9::setColor(const lw::Color& color){
        for ( int i = 0; i < 9; ++i ){
            _pSprites[i]->setColor(color);
        }
	}
	void Sprite9::update(){
		float midW = std::max(0.f, _width - _uvW[0] - _uvW[2]);
		float midH = std::max(0.f, _height - _uvH[0] - _uvH[2]);
		_x[1] = _x[0] + _uvW[0];
		_x[2] = _x[1] + midW;
		_y[1] = _y[0] + _uvH[0];
		_y[2] = _y[1] + midH;
        
		_w[0] = (float)_uvW[0];
		_w[1] = midW;
		_w[2] = (float)_uvW[2];
		_h[0] = (float)_uvH[0];
		_h[1] = midH;
		_h[2] = (float)_uvH[2];
        
        for ( int i = 0; i < 9; ++i ){
            _pSprites[i]->setPos(_x[i/3], _y[i%3]);
            _pSprites[i]->setSize(_w[i/3], _h[i%3]);
        }
	}
	void Sprite9::draw(){
		if ( _needUpdate ){
			update();
			_needUpdate = false;
		}
        for ( int i = 0; i < 9; ++i ){
            _pSprites[i]->draw();
        }
	}
    
    //===============================================
    
    void spriteInit(){
        _pvb = new SpriteVertexBuffer();
    }
    
    void spriteExit(){
        if ( _pvb ){
            delete _pvb;
            _pvb = NULL;
        }
    }
    
    void spriteFlush(){
        _pvb->flush();
    }
    
    void spriteCollectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId){
        _pvb->collectVetices(vertices, numVertices, color, blendMode, textureId);
    }
    
    void addAtlas(const char *file){
        tinyxml2::XMLDocument doc;
        doc.LoadFile(_f(file));
        const tinyxml2::XMLElement *pElem = doc.RootElement();
        pElem = pElem->FirstChildElement("image");
        while ( pElem ){
            const char *name = pElem->Attribute("name");
            AtlasInfo atlas;
            atlas.file = pElem->Attribute("file");
            pElem->QueryIntAttribute("x", &atlas.u);
            pElem->QueryIntAttribute("y", &atlas.v);
            pElem->QueryIntAttribute("w", &atlas.w);
            pElem->QueryIntAttribute("h", &atlas.h);
            _atlasMap[name] = atlas;
            
            pElem = pElem->NextSiblingElement();
        }
    }
    
} //namespace lw

