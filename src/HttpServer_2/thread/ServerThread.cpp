//
// Created by brakulla on 3/10/19.
//

#include "ServerThread.h"

ServerThread::ServerThread(int threadId)
    : _running(false), _executing(false), _threadId(threadId)
{
    printf("Thread :: Initialized - %d\n", _threadId);
}
ServerThread::~ServerThread()
{
    _queue.stop();
    if (_running)
        _running = false;
    if (_executing)
        _executing = false;
    if (_thread.joinable())
        _thread.join();
}
int ServerThread::getThreadId()
{
    return _threadId;
}
void ServerThread::start()
{
    printf("Thread :: Starting - %d\n", _threadId);
    std::unique_lock lock(_mutex);
    _running = true;
    _thread = std::thread(&ServerThread::run, this);
}
void ServerThread::run()
{
    while (_running) {
        if (!_queue.waitPop())
            continue;
        auto func = _queue.pop();
        std::unique_lock lock(_mutex);
        _executing = true;
        printf("Thread :: Starting execution - %d\n", _threadId);
        func();
        printf("Thread :: Executing finished, going idle - %d\n", _threadId);
        _executing = false;
    }
}
void ServerThread::stop()
{
    if (_running)
        printf("Thread :: Stopping - %d\n", _threadId);
    _running = false;
}
bool ServerThread::isExecuting()
{
    std::unique_lock lock(_mutex);
    return _executing;
}
void ServerThread::execute(std::function<void()> func)
{
    printf("Thread :: New execution received - %d\n", _threadId);
    std::unique_lock lock(_mutex);
    _queue.push(func);
}