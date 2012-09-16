#ifndef __LW_BUTTON_H__
#define __LW_BUTTON_H__

#include "lwtf/UI/lwView.h"

namespace lw{
    
    class Sprite;
    class Sprite9;
    class Button;
    class Button9;
    class LabelBM;
    
    class ButtonCallback{
	public:
		virtual ~ButtonCallback(){}
		virtual void vDown(lw::Button* pButton){}
		virtual void vClick(lw::Button* pButton){}
        virtual void vDown(lw::Button9* pButton){}
		virtual void vClick(lw::Button9* pButton){}
	};
    
    struct ButtonDef{
		ButtonDef(ButtonCallback* callback, View* parent, const char* file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h);
        ButtonCallback* callback;
        View* parent;
		const char* file;
		int uUp, vUp, uDown, vDown, uDisable, vDisable;
		int w, h;
	};
    
    class Button : public View{
    public:
        static Button* create(ButtonDef& def);
        static Button* create(ButtonCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h);
        ~Button();
        void setExt(int top, int bottom, int left, int right);
        
    private:
        Button(ButtonCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h, bool &ok);
        virtual bool vEvent(const lw::TouchEvent& evt);
        virtual void vDraw();
        virtual void vUpdatePos();
        
        Sprite *_pSptUp, *_pSptDown, *_pSptDisable;
        ButtonCallback *_pCallback;
        const lw::TouchEvent *_pEvt;
        bool _down;
        bool _tracing;
        int _extTop, _extBottom, _extLeft, _extRight;
    };
    
    class Button9 : public View{
    public:
        static Button9* create(ButtonCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w1, int w2, int w3, int h1, int h2, int h3, const char* font);
        ~Button9();
        void setSize(float w, float h);
        void setExt(int top, int bottom, int left, int right);
        LabelBM* getLabel();
        
    private:
        Button9(ButtonCallback* pCallback, View* pParent, const char* file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w1, int w2, int w3, int h1, int h2, int h3, const char* font, bool &ok);
        virtual bool vEvent(const lw::TouchEvent& evt);
        virtual void vDraw();
        virtual void vUpdatePos();
        
        Sprite9 *_pSptUp, *_pSptDown, *_pSptDisable;
        LabelBM *_pLabel;
        ButtonCallback *_pCallback;
        const lw::TouchEvent *_pEvt;
        bool _down;
        bool _tracing;
        int _extTop, _extBottom, _extLeft, _extRight;
    };


} //namespace lw

#endif //__LW_BUTTON_H__