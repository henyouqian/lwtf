#include "lwApp.h"
#include "lwTask.h"
#include "lwSprite.h"
#include "lwSound.h"
#include "lwView.h"
#import <UIKit/UIKit.h>

namespace lw {
    
    App::App(){
        spriteInit();
        soundInit();
        updateInfo();
    }

    App::~App(){
        spriteExit();
        soundExit();
    }

    void App::main(){
        updateInfo();
        taskMain();
    }

    void App::draw(){
        taskDraw();
        viewDraw();
        spriteFlush();
    }
    
    void App::updateInfo(){
        _screenScale = [UIScreen mainScreen].scale;
        CGSize sz = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds.size;
        _viewW = sz.width;
        _viewH = sz.height;
    }
    
    void App::getViewSize(float &w, float &h){
        w = _viewW; h = _viewH;
    }
    
    float App::getScreenScale(){
        return _screenScale;
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
