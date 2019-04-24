//
// Created by brakulla on 3/10/19.
//

#include <thread/ServerThreadPool.h>

#include "ServerThreadPool.h"

ServerThreadPool::ServerThreadPool()
    : _lastId(0), _maxThreadSize(5), _timeoutDuration(5)
{
    auto conf = Configuration::getConf();
    if (conf.end() != conf.find("Thread")) {
        auto threadConf = conf["Thread"];
        if (threadConf.end() != threadConf.find("MaxThreadSize"))
            _maxThreadSize = threadConf["MaxThreadSize"].get<int>();
        if (threadConf.end() != threadConf.find("ThreadTimeout"))
            _timeoutDuration = threadConf["ThreadTimeout"];
    }
}

void ServerThreadPool::startNewOperation(std::function<void()> func)
{
    printf("ThreadPool :: Starting new operation\n");

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
            SimpleTimer::getInstance().stop(item.first);
            foundIdleThread = true;
            item.second->execute(func);
            SimpleTimer::getInstance().start(item.first, _timeoutDuration);
        }
    }
    _dataMutex.unlock();

    if (!foundIdleThread) {
        std::shared_ptr<ServerThread> thread = std::make_shared<ServerThread>(_lastId++);
        printf("ThreadPool :: New thread created with id: %d\n", thread->getThreadId());
        int id = SimpleTimer::getInstance().insert([=]
                               {
                                   _dataMutex.lock();
                                   printf("ThreadPool :: Deleting idle thread: %d\n", thread->getThreadId());
                                   _threadList.erase(id);
                                   _dataMutex.unlock();
                                   _condVar.notify_one();
                               });
        _dataMutex.lock();
        _threadList.insert(std::make_pair(id, thread));
        _dataMutex.unlock();
        thread->start();
        thread->execute(func);
        SimpleTimer::getInstance().start(id, _timeoutDuration);
    }
}