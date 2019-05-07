//
// Created by brakulla on 3/10/19.
//

#include "ServerThreadPool.h"

ServerThreadPool::ServerThreadPool()
    : timeoutOccurred(nullptr), _lastId(0), _maxThreadSize(5), _timeoutDuration(5)
{
    _maxThreadSize = (int)Configuration::getValue("Thread.MaxThreadSize", 5).toFloat();
    _timeoutDuration = (int)Configuration::getValue("Thread.ThreadTimeout", 5).toFloat();

    timeoutOccurred.setSlotFunction(
        std::bind(&ServerThreadPool::timeoutOnThread, this, std::placeholders::_1));
    SimpleTimer::getInstance().timeout.connect(timeoutOccurred);
}

void ServerThreadPool::startNewOperation(std::function<void()> func)
{
//    printf("ThreadPool :: Starting new operation\n");

    bool foundIdleThread = false;
    _dataMutex.lock();
    auto currentThreadSize = _threadList.size();
    _dataMutex.unlock();
    if (_maxThreadSize == currentThreadSize) {
        printf("ThreadPool :: Max thread size is reached, waiting for another thread to finish\n");
        std::unique_lock lock(_condMutex);
        _condVar.wait(lock, [&]
        {
            return _threadList.size() != _maxThreadSize;
        });
    }

    _dataMutex.lock();
    for (auto &item: _threadList) {
        if (!item.second->isExecuting()) {
            printf("ThreadPool :: Idle thread found, executing in it\n");
            foundIdleThread = true;
            item.second->execute(func);
            SimpleTimer::getInstance().restart(item.first, _timeoutDuration);
        }
    }
    _dataMutex.unlock();

    if (!foundIdleThread) {
        std::shared_ptr<ServerThread> thread = std::make_shared<ServerThread>(_lastId++);
        printf("ThreadPool :: New thread created with id: %d\n", thread->getThreadId());
        int id = SimpleTimer::getInstance().start(_timeoutDuration);
//        printf("ThreadPool :: new timer created with id %d\n", id);
        _dataMutex.lock();
        _threadList.insert(std::make_pair(id, thread));
        _dataMutex.unlock();
        thread->start();
        thread->execute(func);
        SimpleTimer::getInstance().restart(id, _timeoutDuration);
    }
}

void ServerThreadPool::timeoutOnThread(int id)
{
//    printf("ThreadPool :: timeout received with id: %d\n", id);
    auto thread = _threadList.find(id);
    if (_threadList.end() != thread) {
        _dataMutex.lock();
        printf("ThreadPool :: Deleting idle thread: %d\n", thread->second->getThreadId());
        _threadList.erase(id);
        _dataMutex.unlock();
        _condVar.notify_one();
    }
}