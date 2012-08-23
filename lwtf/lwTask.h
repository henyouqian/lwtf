#ifndef __LW_TASK_H__
#define __LW_TASK_H__

namespace lw {
    
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
        
        int _priority;
        bool _isVisible;
        bool _isPaused;
        State _state;
        
    public:
        void main();
        void draw();
        void updateState();
    };
    
    void taskMain();
    void taskDraw();
    
} //namespace lw

#endif //__LW_TASK_H__