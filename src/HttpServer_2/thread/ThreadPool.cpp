//
// Created by brakulla on 3/10/19.
//

#include <thread/ThreadPool.h>

#include "ThreadPool.h"

ThreadPool::ThreadPool() : _lastId(0), _maxThreadSize(5), _timeoutDuration(5) {
  auto conf = Configuration::getConf();
  if (conf.end() != conf.find("Thread")) {
    auto threadConf = conf["Thread"];
    if (threadConf.end() != threadConf.find("MaxThreadSize"))
      _maxThreadSize = threadConf["MaxThreadSize"].get<int>();
    if (threadConf.end() != threadConf.find("ThreadTimeout"))
      _timeoutDuration = threadConf["ThreadTimeout"];
  }
}
void ThreadPool::startNewOperation(std::function<void()> func) {
  spdlog::trace("ThreadPool :: Starting new operation");

  bool foundIdleThread = false;
  _dataMutex.lock();
  auto currentThreadSize = _threadList.size();
  _dataMutex.unlock();
  if (_maxThreadSize == currentThreadSize) {
    spdlog::warn("ThreadPool :: Max thread size is reached, waiting for another thread to finish");
    std::unique_lock lock(_condMutex);
    _condVar.wait(lock, [&]{
      return _threadList.size() != _maxThreadSize;
    });
  }

  _dataMutex.lock();
  for (auto &item: _threadList) {
    if (!item.second->isExecuting()) {
      spdlog::debug("ThreadPool :: Idle thread found, executing in it");
      _timer.stop(item.first);
      foundIdleThread = true;
      item.second->execute(func);
      _timer.start(item.first, _timeoutDuration);
    }
  }
  _dataMutex.unlock();

  if (!foundIdleThread) {
    spdlog::debug("ThreadPool :: Creating new thread");
    std::shared_ptr<Thread> thread = std::make_shared<Thread>();
    int id = _timer.insert([&] {
      spdlog::trace("ThreadPool :: Timeout occured, deleting thread");
      _dataMutex.lock();
      spdlog::trace("ThreadPool :: Locked for erase");
      _threadList.erase(id);
      spdlog::trace("ThreadPool :: Erased");
      _dataMutex.unlock();
      spdlog::trace("ThreadPool :: Unlocked after erased");
      _condVar.notify_one();
      spdlog::trace("ThreadPool :: Notified");
    });
    _dataMutex.lock();
    _threadList.insert(std::make_pair(id, thread));
    _dataMutex.unlock();
    thread->start();
    spdlog::debug("ThreadPool :: Executing function on thread");
    thread->execute(func);
    _timer.start(id, _timeoutDuration);
  }
}