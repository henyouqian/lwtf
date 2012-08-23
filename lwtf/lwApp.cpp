#include "lwApp.h"
#include "lwTask.h"

namespace lw {
    
    App::App(){
        
    }

    App::~App(){
        
    }

    void App::main(){
        taskMain();
    }

    void App::draw(){
        taskDraw();
    }

} //namespace lw