#include "lwTask.h"
#include "lwLog.h"

namespace lw {
    
    class TaskMgr{
    public:
        void add(Task* pTask);
        void main();
        void draw();
        void updateState();
        //bool hasRunningTask();
        bool event(const TouchEvent& evt);
        
    private:
        std::list<Task*> _tasks;
    };
    
    void TaskMgr::add(Task* pTask){
        std::list<Task*>::iterator it = _tasks.begin();
        std::list<Task*>::iterator itend = _tasks.end();
        bool inserted = false;
        for ( ; it != itend; ++it ){
            if ( (*it)->getPriority() >= pTask->getPriority() ){
                _tasks.insert(it, pTask);
                inserted = true;
                break;
            }
        }
        if ( !inserted ){
            _tasks.push_back(pTask);
        }
    }
    
    void TaskMgr::main(){
        std::list<Task*>::iterator it = _tasks.begin();
        std::list<Task*>::iterator itend = _tasks.end();
        for ( ; it != itend; ++it ){
            (*it)->main();
        }
    }
    
    void TaskMgr::draw(){
        std::list<Task*>::iterator it = _tasks.begin();
        std::list<Task*>::iterator itend = _tasks.end();
        for ( ; it != itend; ++it ){
            (*it)->draw();
        }
    }
    
    void TaskMgr::updateState(){
        std::list<Task*>::iterator it = _tasks.begin();
        std::list<Task*>::iterator itend = _tasks.end();
        for ( ; it != itend; ++it ){
            (*it)->updateState();
        }
    }
    
    bool TaskMgr::event(const TouchEvent& evt){
        std::list<Task*>::iterator it = _tasks.begin();
        std::list<Task*>::iterator itend = _tasks.end();
        for ( ; it != itend; ++it ){
            if ( (*it)->event(evt) ){
                return true;
            }
        }
        return false;
    }
    
//    bool TaskMgr::hasRunningTask(){
//        std::list<Task*>::iterator it = _tasks.begin();
//        std::list<Task*>::iterator itend = _tasks.end();
//        for ( ; it != itend; ++it ){
//            if ( (*it)->getState() == Task::RUNNING ){
//                return true;
//            }
//        }
//        return false;
//    }
    
    TaskMgr _taskMgr;
    
    //===================================
    Task::Task(int priority)
    :_priority(priority), _state(IDLE), _isVisible(true), _isPaused(false){
        _taskMgr.add(this);
    }
    
    Task::~Task(){
        
    }
    
    void Task::start(){
        if ( _state != IDLE ){
            lwerror("Task state is not idle");
            return;
        }
        _isVisible = true;
        _isPaused = false;
        _state = STARTING;
    }
    
    void Task::stop(){
        if ( _state != RUNNING ){
            lwerror("Task state is not running");
            return;
        }
        _state = STOPING;
    }
    
    void Task::main(){
        if ( _state == RUNNING && !_isPaused ){
            vMain();
        }
    }
    
    void Task::draw(){
        if ( _state == RUNNING && _isVisible ){
            vDraw();
        }
    }
    
    bool Task::event(const TouchEvent& evt){
        if ( _state == RUNNING && _isVisible && !_isPaused ){
            return vEvent(evt);
        }
        return false;
    }
    
    void Task::updateState(){
        if ( _state == STARTING ){
            _state = RUNNING;
            vBegin();
        }else if ( _state == STOPING ){
            _state = IDLE;
            vEnd();
        }
    }
    
    void Task::show(){
        _isVisible = true;
    }
    
    void Task::hide(){
        _isVisible = false;
    }
    
    void Task::pause(){
        _isPaused = true;
    }
    
    void Task::resume(){
        _isPaused = false;
    }
    
    int Task::getPriority(){
        return _priority;
    }
    
    bool Task::isVisible(){
        return _isVisible;
    }
    
    bool Task::isPaused(){
        return _isPaused;
    }
    
    void Task::switchTo(Task* pTask){
        stop();
        pTask->start();
    }
    
    Task::State Task::getState(){
        return _state;
    }
    
    void taskMain(){
        _taskMgr.main();
    }
    
    void taskDraw(){
        _taskMgr.draw();
    }
    
    void taskUpdateStatus(){
        _taskMgr.updateState();
    }
    
    bool taskEvent(const TouchEvent& evt){
        return _taskMgr.event(evt);
    }
    
} //namespace lw
