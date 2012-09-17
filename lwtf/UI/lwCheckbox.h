#ifndef __LW_CHECKBOX_H__
#define __LW_CHECKBOX_H__

#include "lwtf/UI/lwView.h"

namespace lw{
    
    class Sprite;
    class Checkbox;
    
    class CheckboxCallback{
	public:
		virtual ~CheckboxCallback(){}
		virtual void vCheck(lw::Checkbox* pCheckbox){}
    };
    
    struct CheckboxDefUV{
		CheckboxDefUV(CheckboxCallback* callback, View* parent, const char* file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h);
        CheckboxCallback* callback;
        View* parent;
		const char* file;
		int uUp, vUp, uDown, vDown, uDisable, vDisable;
		int w, h;
	};
    
    struct CheckboxDefAtlas{
        CheckboxDefAtlas(CheckboxCallback* callback, View* parent, const char *atlasUp, const char *atlasDown, const char *atlasDisable);
        CheckboxCallback* callback;
        View* parent;
		const char *atlasUp, *atlasDown, *atlasDisable;
    };
    
    class Checkbox : public View{
    public:
        static Checkbox* create(CheckboxDefUV& def);
        static Checkbox* create(CheckboxDefAtlas& def);
        static Checkbox* create(CheckboxCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h);
        static Checkbox* create(CheckboxCallback* pCallback, View* pParent, const char *atlasUp, const char *atlasDown, const char *atlasDisable);
        ~Checkbox();
        void setExt(int top, int bottom, int left, int right);
        void check(bool b);
        bool isCheck();
        
    private:
        Checkbox(CheckboxCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h, bool &ok);
        Checkbox(CheckboxCallback* pCallback, View* pParent, const char *atlasUp, const char *atlasDown, const char *atlasDisable, bool &ok);
        virtual bool vEvent(const lw::TouchEvent& evt);
        virtual void vDraw();
        virtual void vUpdatePos();
        
        Sprite *_pSptUp, *_pSptDown, *_pSptDisable;
        CheckboxCallback *_pCallback;
        const lw::TouchEvent *_pEvt;
        bool _check;
        int _extTop, _extBottom, _extLeft, _extRight;
    };

} //namespace lw

#endif //__LW_CHECKBOX_H__