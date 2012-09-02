#include "stdafx.h"
#include "lwView.h"

namespace lw{

    View::View(View* pParent)
    :_visible(true), _enable(true), _posX(0.f), _posY(0.f), _screenX(0.f), _screenY(0.f), _pParent(pParent), _w(0.f), _h(0.f){
        updateScreenPos();
        if ( pParent ){
            pParent->addChild(this);
        }
    }
    
    View::~View(){
        std::list<View*>::iterator it = _children.begin();
        std::list<View*>::iterator itend = _children.end();
        for ( ; it != itend; ++it ){
            delete (*it);
        }
    }
    
    bool View::event(const lw::TouchEvent& evt){
        if ( _enable ){
            std::list<View*>::iterator it = _children.begin();
            std::list<View*>::iterator itend = _children.end();
            for ( ; it != itend; ++it ){
                if ( (*it)->event(evt) ){
                    return true;
                }
            }
            return vEvent(evt);
        }
        return false;
    }
    
    void View::draw(){
        if ( _visible ){
            vDraw();
            std::list<View*>::iterator it = _children.begin();
            std::list<View*>::iterator itend = _children.end();
            for ( ; it != itend; ++it ){
                (*it)->draw();
            }
        }
    }
    
    void View::show(bool b){
        _visible = b;
    }
    
    bool View::isVisible(){
        return _visible;
    }
    
    void View::enable(bool b){
        _enable = b;
    }
    
    bool View::isEnable(){
        return _enable;
    }
    
    void View::setPos(float x, float y){
        if ( x != _posX || y != _posY ){
            _posX = x;
            _posY = y;
            updateScreenPos();
            vUpdatePos();
        }
    }
    
    void View::getPos(float &x, float &y){
        x = _posX; y = _posY;
    }
    
    void View::updateScreenPos(){
        if ( _pParent ){
            _pParent->getScreenPos(_screenX, _screenY);
        }else{
            _screenX = _screenY = 0;
        }
        _screenX += _posX;
        _screenY += _posY;
        std::list<View*>::iterator it = _children.begin();
        std::list<View*>::iterator itend = _children.end();
        for ( ; it != itend; ++it ){
            (*it)->updateScreenPos();
        }
    }
    
    void View::getScreenPos(float &x, float &y){
        x = _screenX; y = _screenY;
    }
    
    void View::addChild(View* pView){
        _children.push_back(pView);
    }
    

} //namespace lw