#include "lwSprite.h"
#include "lwTexture.h"
#include "lwShader.h"
#include "lwApp.h"

namespace lw {
    
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
    
	Sprite* Sprite::create(const char *file){
        bool ok = false;
        Sprite* p = new Sprite(file, ok);
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
    
    Sprite::Sprite(const char *file, bool &ok){
        ok = false;
        _pTextureRes = TextureRes::create(file);
        if ( _pTextureRes ){
            ok = true;
            _ancX = _ancY = 0.f;
            _posX = _posY = 0.f;
            _rotate = 0.f;
            _scaleX = _scaleY = 1.f;
            _color.set(255, 255, 255, 255);
            setUV(0, 0, -1, -1);
            _blendMode = BLEND_NORMAL;
            _needUpdate = true;
        }
    }
    
    Sprite::~Sprite(){
        if ( _pTextureRes ){
			_pTextureRes->release();
		}
    }
    
    void Sprite::setUV(float u, float v, float w, float h){
        float texW = (float)_pTextureRes->getWidth();
        float texH = (float)_pTextureRes->getHeight();
        
        if ( w < 0 ){
			_w = texW;
		}else{
			_w = w;
		}
		if ( h < 0 ){
			_h = texH;
		}else{
			_h = h;
		}
        _u1 = u/texW;
		_v1 = v/texH;
        _u2 = (u+_w)/texW;
		_v2 = (v+_h)/texH;
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
        setScale(w/_pTextureRes->getWidth(), h/_pTextureRes->getHeight());
    }
    
    void Sprite::getSize(float &w, float &h){
        w = _pTextureRes->getWidth()*_scaleX;
        h = _pTextureRes->getHeight()*_scaleY;
    }
    
    void Sprite::setColor(const Color& color){
        _color = color;
    }
    
    void Sprite::setBlendMode(BlendMode blendMode){
        _blendMode = blendMode;
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
        cml::Matrix4 m, m1;
		cml::matrix_translation(m, _posX, _posY, 0.f);
		if ( _rotate != 0.f ){
			cml::matrix_rotation_world_z(m1, _rotate);
			m *= m1;
		}
		if ( _scaleX != 1.f || _scaleY != 1.f ){
			cml::matrix_scale(m1, _scaleX, _scaleY, 1.f);
			m *= m1;
		}
        
        float posX1 = -_ancX;
		float posY1 = -_ancY;
		float posX2 = posX1 + _w;
		float posY2 = posY1 + _h;
        
		cml::Vector3 p1(posX1, posY1, 0.f);
		p1 = cml::transform_point(m, p1);
		cml::Vector3 p2(posX1, posY2, 0.f);
		p2 = cml::transform_point(m, p2);
		cml::Vector3 p3(posX2, posY1, 0.f);
		p3 = cml::transform_point(m, p3);
		cml::Vector3 p4(posX2, posY2, 0.f);
		p4 = cml::transform_point(m, p4);
        
        _vertexPos[0][0] = p1[0];
		_vertexPos[0][1] = -p1[1];
        
        _vertexPos[1][0] = p2[0];
		_vertexPos[1][1] = -p2[1];
        
        _vertexPos[2][0] = p3[0];
		_vertexPos[2][1] = -p3[1];
        
        _vertexPos[3][0] = p4[0];
		_vertexPos[3][1] = -p4[1];
    }
    
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
    
    
} //namespace lw

