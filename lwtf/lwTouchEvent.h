#ifndef __LW_TOUCH_EVENT_H__
#define __LW_TOUCH_EVENT_H__

namespace lw{
	
	struct TouchEvent{
		enum Type{
			TOUCH,
			UNTOUCH,
			MOVE,
		};
		Type type;
		short x, y, prx, pry;
	};
    
    void onTouchEvent(lw::TouchEvent::Type type, short x, short y, short prx, short pry);
    void cancelAllTouchEvent();

} //namespace lw

#endif //__LW_TOUCH_EVENT_H__