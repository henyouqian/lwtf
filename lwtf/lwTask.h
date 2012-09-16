#ifndef __LW_TASK_H__
#define __LW_TASK_H__

namespace lw {
    
    struct TouchEvent;
    
	class Task
	{
	public:
		Task(int priority = 0);
		virtual ~Task();
        
		void start();
		void stop();
		void show();
        void hide();
        void pause();
        void resume();
        int getPriority();
        bool isVisible();
        bool isPaused();
        void switchTo(Task* pTask);
        
        enum State{
            IDLE,
            STARTING,
            RUNNING,
            STOPING,
        };
        State getState();
        
    private:
        virtual void vBegin(){};
		virtual void vEnd(){};
        virtual void vMain(){};
        virtual void vDraw(){};
        virtual bool vEvent(const lw::TouchEvent& evt){return false;}
        
        int _priority;
        bool _isVisible;
        bool _isPaused;
        State _state;
        
    public:
        void main();
        void draw();
        void updateState();
        bool event(const lw::TouchEvent& evt);
    };
    
    void taskMain();
    void taskDraw();
    bool taskEvent(const lw::TouchEvent& evt);
    
} //namespace lw

#endif //__LW_TASK_H__