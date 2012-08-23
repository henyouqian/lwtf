#ifndef __LW_APP_H__
#define __LW_APP_H__

#include "lwtf/lwSingleton.h"

namespace lw {
    
    class App : public lw::Singleton<App>{
    public:
        App();
        virtual ~App();
        
        void main();
        void draw();
    };
    
} //namespace lw


#endif //__LW_APP_H__