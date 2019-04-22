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
            _timer.stop(item.first);
            foundIdleThread = true;
            item.second->execute(func);
            _timer.start(item.first, _timeoutDuration);
        }
    }
    _dataMutex.unlock();

    if (!foundIdleThread) {
        printf("ThreadPool :: Creating new thread\n");
        std::shared_ptr<ServerThread> thread = std::make_shared<ServerThread>();
        int id = _timer.insert([&]
                               {
                                   printf("ThreadPool :: Timeout occured, deleting thread\n");
                                   _dataMutex.lock();
                                   printf("ThreadPool :: Locked for erase\n");
                                   _threadList.erase(id);
                                   printf("ThreadPool :: Erased\n");
                                   _dataMutex.unlock();
                                   printf("ThreadPool :: Unlocked after erased\n");
                                   _condVar.notify_one();
                                   printf("ThreadPool :: Notified\n");
                               });
        _dataMutex.lock();
        _threadList.insert(std::make_pair(id, thread));
        _dataMutex.unlock();
        thread->start();
        printf("ThreadPool :: Executing function on thread\n");
        thread->execute(func);
        _timer.start(id, _timeoutDuration);
    }
}