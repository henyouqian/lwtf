#ifndef __LW_SPRITE_H__
#define __LW_SPRITE_H__

#include "lwtf/lwColor.h"
#include "cml/cml.h"

namespace lw{
    
    class TextureRes;
    
    enum BlendMode{
        BLEND_NONE,
        BLEND_NORMAL,
        BLEND_ADD,
        BLEND_MUL,
    };
    struct VertexPosUV{
        float x, y, z;
        float u, v;
    };
    
	class Sprite{
    public:
        static Sprite* create(const char* file);
        ~Sprite();
        void setUV(float u, float v, float w, float h);
        void getUV(float &u, float &v, float &w, float &h);
		void setAnchor(float x, float y);
        void getAnchor(float &x, float &y);
        void setPos(float x, float y);
		void getPos(float &x, float &y);
        void setRotate(float r);
		float getRotate();
		void setScale(float x, float y);
		void getScale(float &x, float &y);
		void setSize(float w, float h);
		void getSize(float &w, float &h);
        void setColor(const Color &color);
        void setBlendMode(BlendMode blendMode);
        void collect();
        
    private:
        TextureRes* _pTextureRes;
        float _ancX, _ancY;
		float _posX, _posY;
		float _rotate;
		float _scaleX, _scaleY;
        Color _color;
        float _u, _v, _w, _h;
        float _u1, _v1, _u2, _v2;
        cml::Vector2 _vertexPos[4];
        bool _needUpdate;
        BlendMode _blendMode;
        
        Sprite(const char *file, bool &ok);
        void update();
    };
    
    void spriteInit();
    void spriteClean();
    void spriteFlush();
	
} //namespace lw

#endif //__LW_SPRITE_H__