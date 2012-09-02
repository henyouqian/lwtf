#ifndef __LW_FONT_H__
#define __LW_FONT_H__

#include "lwtf/lwColor.h"

namespace lw{
    
    enum LabelAlign{
		ALIGN_TOP_LEFT,
		ALIGN_TOP_MID,
		ALIGN_TOP_RIGHT,
		ALIGN_MID_LEFT,
		ALIGN_MID_MID,
		ALIGN_MID_RIGHT,
		ALIGN_BOTTOM_LEFT,
		ALIGN_BOTTOM_MID,
		ALIGN_BOTTOM_RIGHT,
	};
    
    class FontRes;
	
    class LabelBM{
	public:
		static LabelBM* create(const char *fntFile);
		~LabelBM();
        
        void draw();
		void setPos(float x, float y);
		void setAlign(LabelAlign align);
		void setText(const wchar_t *text);
        void setRotate(float rotate);
		void setScale(float scaleX, float scaleY);
		void setColor(const lw::Color& color);
		const wchar_t* getText();
		void getPos(float &x, float &y);
        void getSize(float &w, float &h);
        
	private:
		LabelBM(const char *fntFile, bool &ok);
		void update();
        
		FontRes* _pRes;
        std::wstring _text;
		LabelAlign _align;
		float _posX, _posY;
        float _scaleX, _scaleY;
		float _rotate;
        lw::Color _color;
        float _width, _height;
		
		bool _needUpdate;
	};
    
} //namespace lw

#endif //__LW_FONT_H__