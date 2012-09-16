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