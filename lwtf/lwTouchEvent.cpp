#include "stdafx.h"
#include "lwTouchEvent.h"
#include "lwTask.h"
#include "lwView.h"
#include "lwApp.h"

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
                if ( !viewEvent(_events.back()) )
                    taskEvent(_events.back());
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
                        if ( !viewEvent(evt) )
                            taskEvent(evt);
                        break;
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
                        if ( !viewEvent(evt) )
                            taskEvent(evt);
                        _events.erase(it);
                        break;
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
        _mgr.onTouchEvent(type, _as(x), _as(y), _as(prx), _as(pry));
    }
    
    void cancelAllTouchEvent(){
        _mgr.cancelAllTouchEvent();
    }
	

} //namespace lw