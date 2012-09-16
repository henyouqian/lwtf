#ifndef __LW_VIEW_H__
#define __LW_VIEW_H__

namespace lw{
    
    struct TouchEvent;
	
    class View{
    public:
        View(View* pParent);
        virtual ~View();
        void show(bool b);
        void enable(bool b);
        bool event(const lw::TouchEvent& evt);  //return true if event processed and not pass to next;
        void draw();
        void setPos(float x, float y);
        
        bool isVisible();
        bool isEnable();
        void getPos(float &x, float &y);
        void getScreenPos(float &x, float &y);
        
    protected:
        virtual void vShow(bool b){};
        virtual void vEnable(bool){};
        virtual bool vEvent(const lw::TouchEvent& evt){return false;}
        virtual void vDraw(){};
        virtual void vUpdatePos(){};
        
        void updateScreenPos();
        
        std::list<View*> _children;
        View* _pParent;
        bool _enable;
        bool _visible;
        float _posX, _posY;
        float _screenX, _screenY;
        float _w, _h;
        
    public:
        void addChild(View* pView);
        void removeChild(View* pView);
    };
    
    void viewDraw();
    bool viewEvent(const lw::TouchEvent& evt);

} //namespace lw

#endif //__LW_VIEW_H__