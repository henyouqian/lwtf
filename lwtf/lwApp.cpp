#include "lwApp.h"
#include "lwTask.h"
#include "lwSprite.h"

namespace lw {
    
    App::App(){
        spriteInit();
    }

    App::~App(){
        spriteClean();
    }

    void App::main(){
        taskMain();
    }

    void App::draw(){
        taskDraw();
        spriteFlush();
    }
    
    void App::setViewSize(float w, float h){
        _viewW = w; _viewH = h;
    }
    
    void App::getViewSize(float &w, float &h){
        w = _viewW; h = _viewH;
    }

} //namespace lw