#include "stdafx.h"
#include "lwButton.h"
#include "lwSprite.h"
#include "lwTouchEvent.h"
#include "lwLabelBM.h"
#include "lwLog.h"

namespace lw{
    
    ButtonDefUV::ButtonDefUV(ButtonCallback* callback_, View* parent_, const char* file_, int uUp_, int vUp_, int uDown_, int vDown_, int uDisable_, int vDisable_, int w_, int h_){
        callback = callback_;
        parent = parent_;
        file = file_;
        uUp = uUp_;
        vUp = vUp_;
        uDown = uDown_;
        vDown = vDown_;
        uDisable = uDisable_;
        vDisable = vDisable_;
        w = w_;
        h = h_;
    }
    
    ButtonDefAtlas::ButtonDefAtlas(ButtonCallback* callback_, View* parent_, const char *atlasUp_, const char *atlasDown_, const char *atlasDisable_){
        callback = callback_;
        parent = parent_;
        atlasUp = atlasUp_;
        atlasDown = atlasDown_;
        atlasDisable = atlasDisable_;
    }
    
    Button* Button::create(ButtonDefUV& def){
        return create(def.callback, def.parent, def.file, def.uUp, def.vUp, def.uDown, def.vDown, def.uDisable, def.vDisable, def.w, def.h);
    }
    
    Button* Button::create(ButtonDefAtlas& def){
        return create(def.callback, def.parent, def.atlasUp, def.atlasDown, def.atlasDisable);
    }

    Button* Button::create(ButtonCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h){
        bool ok = false;
        Button *p = new Button(pCallback, pParent, file, uUp, vUp, uDown, vDown, uDisable, vDisable, w, h, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL;
    }
    
    Button* Button::create(ButtonCallback* pCallback, View* pParent, const char *atlasUp, const char *atlasDown, const char *atlasDisable){
        bool ok = false;
        Button *p = new Button(pCallback, pParent, atlasUp, atlasDown, atlasDisable, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL;
    }
    
    Button::Button(ButtonCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w, int h, bool &ok)
    :View(pParent), _pCallback(pCallback), _pEvt(NULL), _down(false), _tracing(false)
    ,_extTop(0),_extBottom(0),_extLeft(0),_extRight(0){
        _pSptUp = Sprite::createFromFile(file);
        _pSptDown = Sprite::createFromFile(file);
        _pSptDisable = Sprite::createFromFile(file);
        if ( _pSptUp && _pSptDown && _pSptDisable ){
            _w = w;
            _h = h;
            _pSptUp->setUV((float)uUp, (float)vUp, (float)w, (float)h);
            _pSptDown->setUV((float)uDown, (float)vDown, (float)w, (float)h);
            _pSptDisable->setUV((float)uDisable, (float)vDisable, (float)w, (float)h);
            ok = true;
        }else{
            ok = false;
        }
    }
    
    Button::Button(ButtonCallback* pCallback, View* pParent, const char *atlasUp, const char *atlasDown, const char *atlasDisable, bool &ok)
    :View(pParent), _pCallback(pCallback), _pEvt(NULL), _down(false), _tracing(false)
    ,_extTop(0),_extBottom(0),_extLeft(0),_extRight(0){
        _pSptUp = Sprite::createFromAtlas(atlasUp);
        _pSptDown = Sprite::createFromAtlas(atlasDown);
        _pSptDisable = Sprite::createFromAtlas(atlasDisable);
        if ( _pSptUp && _pSptDown && _pSptDisable ){
            _pSptUp->getSize(_w, _h);
            ok = true;
        }else{
            ok = false;
        }
    }
    
    Button::~Button(){
        if ( _pSptUp ){
            delete _pSptUp;
        }
        if ( _pSptDown ){
            delete _pSptDown;
        }
        if ( _pSptDisable ){
            delete _pSptDisable;
        }
    }
    
    bool Button::vEvent(const lw::TouchEvent& evt){
        if ( _pEvt && _pEvt != &evt ){
            return false;
        }
		switch (evt.type)
		{
            case lw::TouchEvent::TOUCH:
                if ( evt.x >= _screenX-_extLeft && evt.x < _screenX+_w+_extRight 
                    && evt.y >= _screenY-_extTop && evt.y < _screenY+_h+_extBottom ){
                    _tracing = true;
                    _down = true;
                    _pEvt = &evt;
                    if ( _pCallback ){
                        _pCallback->vDown(this);
                    }
                    return true;
                }
                break;
            case lw::TouchEvent::UNTOUCH:{
				if ( _pEvt == &evt ){
					_pEvt = NULL;
					bool isDown = _down;
					_tracing = false;
					_down = false;
					if ( isDown && _pCallback ){
						_pCallback->vClick(this);
						return true;
					}
				}
			}
                break;
            case lw::TouchEvent::MOVE:
                if ( _tracing && _pEvt == &evt ){
                    if ( evt.x >= _screenX-_extLeft && evt.x < _screenX+_w+_extRight 
                        && evt.y >= _screenY-_extTop && evt.y < _screenY+_h+_extBottom ){
						_down = true;
                    }else{
                        _down = false;
                    }
                    return true;
                }
                break;
		}
        return false;
    }
    
    void Button::vDraw(){
        if ( _enable ){
			if ( _down ){
				_pSptDown->draw();
			}else{
				_pSptUp->draw();
			}
		}else{
			_pSptDisable->draw();
		}
    }
    
    void Button::vUpdatePos(){
        _pSptUp->setPos(_screenX, _screenY);
        _pSptDown->setPos(_screenX, _screenY);
        if ( _pSptDisable ){
            _pSptDisable->setPos(_screenX, _screenY);
        }
    }
    
    void Button::setExt(int top, int bottom, int left, int right){
        _extTop = top; _extBottom = bottom; _extLeft = left; _extRight = right;
    }
    
    
    //======================================================
    Button9DefUV::Button9DefUV(ButtonCallback* callback_, View* parent_, const char *file_, int uUp_, int vUp_, int uDown_, int vDown_, int uDisable_, int vDisable_, int w1_, int w2_, int w3_, int h1_, int h2_, int h3_, const char* font_){
        callback = callback_;
        parent = parent_;
        file = file_;
        uUp = uUp_;
        vUp = vUp_;
        uDown = uDown_;
        vDown = vDown_;
        uDisable = uDisable_;
        vDisable = vDisable_;
        w1 = w1_;
        w2 = w2_;
        w3 = w3_;
        h1 = h1_;
        h2 = h2_;
        h3 = h3_;
        font = font_;
    }
    
    Button9DefAtlas::Button9DefAtlas(ButtonCallback* callback_, View* parent_, const char *atlasUp_, const char *atlasDown_, const char *atlasDisable_, int w1_, int w2_, int w3_, int h1_, int h2_, int h3_, const char* font_){
        callback = callback_;
        parent = parent_;
        atlasUp = atlasUp_;
        atlasDown = atlasDown_;
        atlasDisable = atlasDisable_;
        w1 = w1_;
        w2 = w2_;
        w3 = w3_;
        h1 = h1_;
        h2 = h2_;
        h3 = h3_;
        font = font_;
    }
    
    Button9* Button9::create(Button9DefUV& def){
        return create(def.callback, def.parent, def.file, def.uUp, def.vUp, def.uDown, def.vDown, def.uDisable, def.vDisable, def.w1, def.w2, def.w3, def.h1, def.h2, def.h3, def.font);
    }
    
    Button9* Button9::create(Button9DefAtlas& def){
        return create(def.callback, def.parent, def.atlasUp, def.atlasDown, def.atlasDisable, def.w1, def.w2, def.w3, def.h1, def.h2, def.h3, def.font);
    }
    
    Button9* Button9::create(ButtonCallback* pCallback, View* pParent, const char *file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w1, int w2, int w3, int h1, int h2, int h3, const char* font){
        
        bool ok = false;
        Button9 *p = new Button9(pCallback, pParent, file, uUp, vUp, uDown, vDown, uDisable, vDisable, w1, w2, w3, h1, h2, h3, font, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL;
    }
    
    Button9* Button9::create(ButtonCallback* pCallback, View* pParent, const char *atlasUp, const char *atlasDown, const char *atlasDisable, int w1, int w2, int w3, int h1, int h2, int h3, const char* font){
        
        bool ok = false;
        Button9 *p = new Button9(pCallback, pParent, atlasUp, atlasDown, atlasDisable, w1, w2, w3, h1, h2, h3, font, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL; 
    }
    
    Button9::Button9(ButtonCallback* pCallback, View* pParent, const char* file, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, int w1, int w2, int w3, int h1, int h2, int h3, const char* font, bool &ok)
    :View(pParent), _pCallback(pCallback), _pEvt(NULL), _down(false), _tracing(false)
    ,_extTop(0),_extBottom(0),_extLeft(0),_extRight(0){
        _pSptUp = Sprite9::create(file, uUp, vUp, w1, w2, w3, h1, h2, h3);
        _pSptDown = Sprite9::create(file, uDown, vDown, w1, w2, w3, h1, h2, h3);
        _pSptDisable = Sprite9::create(file, uDisable, vDisable, w1, w2, w3, h1, h2, h3);
        if ( _pSptUp == NULL || _pSptDown == NULL || _pSptDisable == NULL ){
            lwerror("sprite create error");
            return;
        }
        if ( font ){
            _pLabel = lw::LabelBM::create(font);
            if ( !_pLabel ){
                lwerror("label create error");
                return;
            }
            _pLabel->setAlign(lw::ALIGN_MID_MID);
        }
        
        _w = w1+w2+w3;
        _h = h1+h2+h3;
        ok = true;
    }
    
    Button9::Button9(ButtonCallback* pCallback, View* pParent, const char *atlasUp, const char *atlasDown, const char *atlasDisable, int w1, int w2, int w3, int h1, int h2, int h3, const char* font, bool &ok)
    :View(pParent), _pCallback(pCallback), _pEvt(NULL), _down(false), _tracing(false)
    ,_extTop(0),_extBottom(0),_extLeft(0),_extRight(0){
        _pSptUp = Sprite9::create(atlasUp, w1, w2, w3, h1, h2, h3);
        _pSptDown = Sprite9::create(atlasDown, w1, w2, w3, h1, h2, h3);
        _pSptDisable = Sprite9::create(atlasDisable, w1, w2, w3, h1, h2, h3);
        if ( _pSptUp == NULL || _pSptDown == NULL || _pSptDisable == NULL ){
            lwerror("sprite create error");
            return;
        }
        if ( font ){
            _pLabel = lw::LabelBM::create(font);
            if ( !_pLabel ){
                lwerror("label create error");
                return;
            }
            _pLabel->setAlign(lw::ALIGN_MID_MID);
        }
        
        _w = w1+w2+w3;
        _h = h1+h2+h3;
        ok = true;
    }
    
    Button9::~Button9(){
        if ( _pSptUp ){
            delete _pSptUp;
        }
        if ( _pSptDown ){
            delete _pSptDown;
        }
        if ( _pSptDisable ){
            delete _pSptDisable;
        }
        if ( _pLabel ){
            delete _pLabel;
        }
    }
    
    bool Button9::vEvent(const lw::TouchEvent& evt){
        if ( _pEvt && _pEvt != &evt ){
            return false;
        }
		switch (evt.type)
		{
            case lw::TouchEvent::TOUCH:
                if ( evt.x >= _screenX-_extLeft && evt.x < _screenX+_w+_extRight 
                    && evt.y >= _screenY-_extTop && evt.y < _screenY+_h+_extBottom ){
                    _tracing = true;
                    _down = true;
                    _pEvt = &evt;
                    if ( _pCallback ){
                        _pCallback->vDown(this);
                    }
                    return true;
                }
                break;
            case lw::TouchEvent::UNTOUCH:{
				if ( _pEvt == &evt ){
					_pEvt = NULL;
					bool isDown = _down;
					_tracing = false;
					_down = false;
					if ( isDown && _pCallback ){
						_pCallback->vClick(this);
						return true;
					}
				}
			}
                break;
            case lw::TouchEvent::MOVE:
                if ( _tracing && _pEvt == &evt ){
                    if ( evt.x >= _screenX-_extLeft && evt.x < _screenX+_w+_extRight 
                        && evt.y >= _screenY-_extTop && evt.y < _screenY+_h+_extBottom ){
						_down = true;
                    }else{
                        _down = false;
                    }
                    return true;
                }
                break;
		}
        return false;
    }
    
    void Button9::vDraw(){
        if ( _enable ){
			if ( _down ){
				_pSptDown->draw();
			}else{
				_pSptUp->draw();
			}
		}else{
			_pSptDisable->draw();
		}
        if ( _pLabel ){
            _pLabel->draw();
        }
    }
    
    void Button9::vUpdatePos(){
        _pSptUp->setPos(_screenX, _screenY);
        _pSptDown->setPos(_screenX, _screenY);
        _pSptDisable->setPos(_screenX, _screenY);
        if ( _pLabel ){
            _pLabel->setPos(_screenX+_w*.5f, _screenY+_h*.5f);
        }
    }
    
    void Button9::setSize(float w, float h){
        _pSptUp->setSize(w, h);
        _pSptDown->setSize(w, h);
        _pSptDisable->setSize(w, h);
        _w = w;
        _h = h;
        if ( _pLabel ){
            _pLabel->setPos(_screenX+_w*.5f, _screenY+_h*.5f);
        }
    }
    
    void Button9::setExt(int top, int bottom, int left, int right){
        _extTop = top; _extBottom = bottom; _extLeft = left; _extRight = right;
    }
    
    LabelBM* Button9::getLabel(){
        return _pLabel;
    }

} //namespace lw