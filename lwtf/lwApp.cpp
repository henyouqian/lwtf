#include "lwApp.h"
#include "lwTask.h"
#include "lwSprite.h"
#include "lwSound.h"
#include "lwView.h"

namespace lw {
    
    App::App(){
        spriteInit();
        soundInit();
    }

    App::~App(){
        spriteExit();
        soundExit();
    }

    void App::main(){
        taskMain();
    }

    void App::draw(){
        taskDraw();
        viewDraw();
        spriteFlush();
    }
    
    void App::setViewSize(float w, float h){
        _viewW = w; _viewH = h;
    }
    
    void App::getViewSize(float &w, float &h){
        w = _viewW; h = _viewH;
    }

} //namespace lw