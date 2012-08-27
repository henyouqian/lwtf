#ifndef __LW_BUTTON_H__
#define __LW_BUTTON_H__

#include "lwtf/UI/lwView.h"

namespace lw{
    
    class Sprite;
    class Button;
    
    class ButtonCallback{
	public:
		virtual ~ButtonCallback(){}
		virtual void vDown(lw::Button* pButton){}
		virtual void vClick(lw::Button* pButton){}
	};
    
    class Button : public View{
    public:
        static Button* create(ButtonCallback* pCallback, View* pParent, const char *file, int w, int h, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable);
        ~Button();
        void setExt(int top, int bottom, int left, int right);
        
    private:
        Button(ButtonCallback* pCallback, View* pParent, const char *file, int w, int h, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, bool &ok);
        virtual bool vEvent(const lw::TouchEvent& evt);
        virtual void vDraw();
        
        Sprite *_pSptUp, *_pSptDown, *_pSptDisable;
        ButtonCallback *_pCallback;
        const lw::TouchEvent *_pEvt;
        bool _down;
        bool _tracing;
        int _extTop, _extBottom, _extLeft, _extRight;
    };

} //namespace lw

#endif //__LW_BUTTON_H__