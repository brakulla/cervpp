//
// Created by brakulla on 3/10/19.
//

#include <thread/Thread.h>

#include "Thread.h"

Thread::Thread()
    : _running(false), _executing(false)
{
    printf("Thread :: Initialized\n");
}
Thread::~Thread()
{
    _queue.stop();
    if (_running)
        _running = false;
    if (_executing)
        _executing = false;
    if (_thread.joinable())
        _thread.join();
}
void Thread::start()
{
    printf("Thread :: Starting\n");
    std::unique_lock lock(_mutex);
    _running = true;
    _thread = std::thread(&Thread::run, this);
}
void Thread::run()
{
    while (_running) {
        if (!_queue.waitPop())
            continue;
        auto func = _queue.pop();
        std::unique_lock lock(_mutex);
        _executing = true;
        printf("Thread :: Starting execution\n");
        func();
        printf("Thread :: Executing finished, going idle\n");
        _executing = false;
    }
}
void Thread::stop()
{
    if (_running)
        printf("Thread :: Stopping\n");
    _running = false;
}
bool Thread::isExecuting()
{
    std::unique_lock lock(_mutex);
    return _executing;
}
void Thread::execute(std::function<void()> func)
{
    printf("Thread :: New execution received\n");
    std::unique_lock lock(_mutex);
    _queue.push(func);
}