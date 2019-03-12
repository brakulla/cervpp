//
// Created by brakulla on 3/10/19.
//

#include <thread/Thread.h>

#include "Thread.h"

Thread::Thread() : _running(false), _executing(false) {
  spdlog::trace("Thread :: Initialized");
}
Thread::~Thread() {
  _queue.stop();
  if (_running)
    _running = false;
  if (_executing)
    _executing = false;
  if (_thread.joinable())
    _thread.join();
}
void Thread::start() {
  spdlog::trace("Thread :: Starting");
  std::unique_lock lock(_mutex);
  _running = true;
  _thread = std::thread(&Thread::run, this);
}
void Thread::run() {
  while(_running) {
    if (!_queue.waitPop())
      continue;
    auto func = _queue.pop();
    std::unique_lock lock(_mutex);
    _executing = true;
    spdlog::trace("Thread :: Starting execution");
    func();
    spdlog::trace("Thread :: Executing finished, going idle");
    _executing = false;
  }
}
void Thread::stop() {
  if (_running)
    spdlog::trace("Thread :: Stopping");
  _running = false;
}
bool Thread::isExecuting() {
  std::unique_lock lock(_mutex);
  return _executing;
}
void Thread::execute(std::function<void()> func) {
  spdlog::trace("Thread :: New execution received");
  std::unique_lock lock(_mutex);
  _queue.push(func);
}