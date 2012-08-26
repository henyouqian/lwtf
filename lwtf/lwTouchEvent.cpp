#include "stdafx.h"
#include "lwTouchEvent.h"
#include "lwTask.h"
#include <list>

namespace lw{
    
    namespace {
        class TouchEventMgr{
        public:
            void onTouchEvent(TouchEvent::Type type, short x, short y, short prx, short pry);
            void cancelAllTouchEvent();
            
        private:
            std::list<TouchEvent> _events;
        };
        
        void TouchEventMgr::onTouchEvent(TouchEvent::Type type, short x, short y, short prx, short pry){
            if ( type == TouchEvent::TOUCH ){
                TouchEvent evt;
                evt.type = type;
                evt.x = x;
                evt.y = y;
                evt.prx = prx;
                evt.pry = pry;
                _events.push_back(evt);
                taskEvent(evt);
            }else if (type == TouchEvent::MOVE){
                std::list<TouchEvent>::iterator it = _events.begin();
                std::list<TouchEvent>::iterator itend = _events.end();
                for ( ; it != itend; ++it ){
                    TouchEvent &evt = (*it);
                    if ( prx == evt.x && pry == evt.y ){
                        evt.type = type;
                        evt.x = x;
                        evt.y = y;
                        evt.prx = prx;
                        evt.pry = pry;
                        taskEvent(evt);
                    }
                }
            }else if ( type == TouchEvent::UNTOUCH ){
                std::list<TouchEvent>::iterator it = _events.begin();
                std::list<TouchEvent>::iterator itend = _events.end();
                for ( ; it != itend; ++it ){
                    TouchEvent &evt = (*it);
                    if ( (prx == evt.x && pry == evt.y)
                        || (x == evt.x && y == evt.y)){
                        evt.type = type;
                        evt.x = x;
                        evt.y = y;
                        evt.prx = prx;
                        evt.pry = pry;
                        taskEvent(evt);
                        _events.erase(it);
                    }
                }
            }
        }
        
        void TouchEventMgr::cancelAllTouchEvent(){
            _events.clear();
        }
        
        TouchEventMgr _mgr;
    }

    void onTouchEvent(TouchEvent::Type type, short x, short y, short prx, short pry){
        _mgr.onTouchEvent(type, x, y, prx, pry);
    }
    
    void cancelAllTouchEvent(){
        _mgr.cancelAllTouchEvent();
    }
	

} //namespace lw