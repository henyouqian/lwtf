#include "lwApp.h"
#include "lwTask.h"
#include "lwSprite.h"
#include "lwSound.h"
#include "lwView.h"
#import <UIKit/UIKit.h>

namespace lw {
    
    App::App(){
        _screenScale = [UIScreen mainScreen].scale;
        spriteInit();
        soundInit();
    }

    App::~App(){
        spriteExit();
        soundExit();
    }

    void App::main(){
        taskUpdateStatus();
        taskMain();
    }

    void App::draw(){
        taskDraw();
        viewDraw();
        spriteFlush();
    }
    
    void App::getViewSize(float &w, float &h){
        w = _viewW; h = _viewH;
    }
    
    void App::getScreenSize(float &w, float &h){
        w = _viewW*_screenScale;
        h = _viewH*_screenScale;
    }
    
    float App::getScreenScale(){
        return _screenScale;
    }
    
    void App::setViewSize(float w, float h){
        _viewW = w;
        _viewH = h;
    }

} //namespace lw

_af::_af(const char* s){
    _s = s;
    if ( lw::App::s().getScreenScale() == 2 ){
        if ( _s.find("_2x.") != std::string::npos ){
            lwerror("alread 2x");
            return;
        }
        int n = _s.find('.');
        std::string ext = &_s[n];
        _s.resize(n);
        _s.append("_2x");
        _s.append(ext);
    }
}
_af::~_af(){
    
}
_af:: operator const char*(){
    return _s.c_str();
}
