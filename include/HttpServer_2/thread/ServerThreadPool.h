/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_THREADPOOL_H
#define CERVPP_THREADPOOL_H

#include <memory>
#include <functional>
#include <thread>
#include <map>
#include <mutex>
#include <condition_variable>

#include "Configuration.h"
#include "ServerThread.h"
#include "SimpleTimer.h"

class ServerThreadPool
{
public:
    ServerThreadPool();
    ~ServerThreadPool() = default;

    void startNewOperation(std::function<void()> func);

    brutils::slot<int> timeoutOccurred;

private: // config
    unsigned int _timeoutDuration;
    unsigned int _maxThreadSize;

private:
    std::mutex _condMutex;
    std::mutex _dataMutex;
    std::condition_variable _condVar;

private:
    unsigned int _lastId;
    std::map<int, std::shared_ptr<ServerThread>> _threadList;

private:
    void timeoutOnThread(int id);
};

#endif //CERVPP_THREADPOOL_H
