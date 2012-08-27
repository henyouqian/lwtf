#include "stdafx.h"
#include "lwtf/lwButton.h"
#include "lwtf/lwSprite.h"
#include "lwtf/lwTouchEvent.h"

namespace lw{

    Button* Button::create(ButtonCallback* pCallback, View* pParent, const char *file, int w, int h, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable){
        bool ok = false;
        Button *p = new Button(pCallback, pParent, file, w, h, uUp, vUp, uDown, vDown, uDisable, vDisable, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL;
    }
    
    Button::Button(ButtonCallback* pCallback, View* pParent, const char *file, int w, int h, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, bool &ok)
    :View(pParent), _pCallback(pCallback), _pEvt(NULL), _down(false), _tracing(false)
    ,_extTop(0),_extBottom(0),_extLeft(0),_extRight(0){
        _pSptUp = Sprite::create(file);
        _pSptDown = Sprite::create(file);
        if ( vDisable >= 0 && uDisable >= 0 ){
            _pSptDisable = Sprite::create(file);
        }else{
            _pSptDisable = NULL;
        }
        if ( _pSptUp && _pSptDown ){
            _w = w;
            _h = h;
            _pSptUp->setUV((float)uUp, (float)vUp, (float)w, (float)h);
            _pSptDown->setUV((float)uDown, (float)vDown, (float)w, (float)h);
            if ( _pSptDisable ){
                _pSptDisable->setUV((float)uDisable, (float)vDisable, (float)w, (float)h);
            }
        }else{
            ok = false;
        }
        ok = true;
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
    
    void Button::setExt(int top, int bottom, int left, int right){
        _extTop = top; _extBottom = bottom; _extLeft = left; _extRight = right;
    }
    

} //namespace lw