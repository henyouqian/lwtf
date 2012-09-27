#ifndef __LW_SPRITE_H__
#define __LW_SPRITE_H__

#include "lwtf/lwColor.h"
#include "cml/cml.h"
#include <OpenGLES/ES2/gl.h>
#include "lwtf/lwApp.h"

namespace lw{
    
    class TextureRes;
    
    enum BlendMode{
        BLEND_NONE,
        BLEND_NORMAL,
        BLEND_ADD,
        BLEND_MUL,
    };
    struct SpriteVertex{
        float x, y, z;
        float u, v;
    };
    
	class Sprite{
    public:
        static Sprite* createFromFile(const char* file);
        static Sprite* createFromAtlas(const char* key);
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
        void setMatrix(cml::Matrix3 m);
        void draw();
        GLuint getGlId();
        
    private:
        void uvInit();
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
        
        Sprite(const char *file, bool fromAtlas, bool &ok);
        void loadFromFile(const char *file);
        void loadFromAtlas(const char *key);
        void update();
    };
    
    class Sprite9{
	public:
		static Sprite9* create(const char* file, int u, int v, int w1, int w2, int w3, int h1, int h2, int h3);
        static Sprite9* create(const char* atlasKey, int w1, int w2, int w3, int h1, int h2, int h3);
		~Sprite9();
		void setPos(float x, float y);
		void setSize(float w, float h);
		void setColor(const lw::Color& color);
		void draw();
		
	private:
		Sprite9(const char* fileName, int u, int v, int w1, int w2, int w3, int h1, int h2, int h3, bool& ok);
        Sprite9(const char* atlasKey, int w1, int w2, int w3, int h1, int h2, int h3, bool& ok);
        void update();
		Sprite* _pSprites[9];
		float _uvX[3], _uvY[3], _uvW[3], _uvH[3];
		float _x[3], _y[3], _w[3], _h[3];
		float _width, _height;
		bool _needUpdate;
	}; 
    
    void spriteInit();
    void spriteExit();
    void spriteFlush();
    void spriteCollectVetices(SpriteVertex *vertices, int numVertices, Color &color, BlendMode blendMode, GLuint textureId);
    
    void addAtlas(const char *file);
	
} //namespace lw

#endif //__LW_SPRITE_H__