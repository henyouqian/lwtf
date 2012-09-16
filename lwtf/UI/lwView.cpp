#include "stdafx.h"
#include "lwView.h"

namespace lw{
    
    namespace{
        View _rootView(NULL);
    }

    View::View(View* pParent)
    :_visible(true), _enable(true), _posX(0.f), _posY(0.f), _screenX(0.f), _screenY(0.f), _w(0.f), _h(0.f){
        if ( pParent == NULL && this != &_rootView ){
            pParent = &_rootView;
        }
        _pParent = pParent;
        updateScreenPos();
        if ( _pParent )
            _pParent->addChild(this);
    }
    
    View::~View(){
        if ( _pParent ){
            _pParent->removeChild(this);
        }
        std::list<View*>::iterator it = _children.begin();
        std::list<View*>::iterator itend = _children.end();
        for ( ; it != itend; ++it ){
            delete (*it);
        }
    }
    
    bool View::event(const lw::TouchEvent& evt){
        if ( _enable && _visible ){
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
    
    void View::removeChild(View* pView){
        _children.remove(pView);
    }
    
    void viewDraw(){
        _rootView.draw();
    }
    
    bool viewEvent(const lw::TouchEvent& evt){
        return _rootView.event(evt);
    }

} //namespace lw