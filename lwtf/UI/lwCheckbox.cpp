#include "stdafx.h"
#include "lwCheckbox.h"
#include "lwSprite.h"
#include "lwTouchEvent.h"

namespace lw{

    Checkbox* Checkbox::create(CheckboxCallback* pCallback, View* pParent, const char *file, int w, int h, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable){
        bool ok = false;
        Checkbox *p = new Checkbox(pCallback, pParent, file, w, h, uUp, vUp, uDown, vDown, uDisable, vDisable, ok);
        if ( p && ok ){
            return p;
        }else if ( p ){
            delete p;
        }
        return NULL;
    }
    
    Checkbox::Checkbox(CheckboxCallback* pCallback, View* pParent, const char *file, int w, int h, int uUp, int vUp, int uDown, int vDown, int uDisable, int vDisable, bool &ok)
    :View(pParent), _pCallback(pCallback), _pEvt(NULL), _check(false)
    ,_extTop(0),_extBottom(0),_extLeft(0),_extRight(0){
        _pSptUp = Sprite::createFromFile(file);
        _pSptDown = Sprite::createFromFile(file);
        if ( vDisable >= 0 && uDisable >= 0 ){
            _pSptDisable = Sprite::createFromFile(file);
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
    
    Checkbox::~Checkbox(){
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
    
    bool Checkbox::vEvent(const lw::TouchEvent& evt){
        if ( _pEvt && _pEvt != &evt ){
            return false;
        }
		
        if ( evt.type == lw::TouchEvent::TOUCH ){
            if ( evt.x >= _screenX-_extLeft && evt.x < _screenX+_w+_extRight 
            && evt.y >= _screenY-_extTop && evt.y < _screenY+_h+_extBottom ){
                _pEvt = &evt;
                return true;
            }
        }else if ( evt.type == lw::TouchEvent::UNTOUCH ){
            if ( _pEvt == &evt ){
                _pEvt = NULL;
                if ( evt.x >= _screenX-_extLeft && evt.x < _screenX+_w+_extRight 
                    && evt.y >= _screenY-_extTop && evt.y < _screenY+_h+_extBottom ){
                    _check = !_check;
                    if ( _pCallback ){
                        _pCallback->vCheck(this);
                    }
                    return true;
                }
            }
        }
        return false;
    }
    
    void Checkbox::vUpdatePos(){
        _pSptUp->setPos(_screenX, _screenY);
        _pSptDown->setPos(_screenX, _screenY);
        if ( _pSptDisable ){
            _pSptDisable->setPos(_screenX, _screenY);
        }
    }
    
    void Checkbox::vDraw(){
        if ( _enable ){
			if ( _check ){
                _pSptDown->draw();
            }else{
                _pSptUp->draw();
            }
		}else{
            if ( _pSptDisable ){
                _pSptDisable->draw();
            }
		}
    }
    
    void Checkbox::check(bool b){
        _check = b;
    }
    
    bool Checkbox::isCheck(){
        return _check;
    }
    
    void Checkbox::setExt(int top, int bottom, int left, int right){
        _extTop = top; _extBottom = bottom; _extLeft = left; _extRight = right;
    }
    

} //namespace lw