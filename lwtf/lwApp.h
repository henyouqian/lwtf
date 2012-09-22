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
        
        void getViewSize(float &w, float &h);
        void getScreenSize(float &w, float &h);
        float getScreenScale();
        
    private:
        float _viewW, _viewH, _screenScale;
        
    public:
        void setViewSize(float w, float h);
    };
    
} //namespace lw

template<typename T> T _as(const T& v){ //auto scale
    return v*lw::App::s().getScreenScale();
}

class _af   //auto file name
{
public:
	_af(const char* fileName);
	~_af();
	operator const char*();
    
private:
    std::string _s;
};


#endif //__LW_APP_H__