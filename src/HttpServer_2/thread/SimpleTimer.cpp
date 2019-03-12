//
// Created by brakulla on 3/10/19.
//

#include <thread/SimpleTimer.h>

#include "SimpleTimer.h"
SimpleTimer::SimpleTimer() : _running(true), _lastId(0) {
  spdlog::trace("SimpleTimer :: Starting timer");
  _thread = std::thread(&SimpleTimer::run, this);
}
SimpleTimer::~SimpleTimer() {
  _running = false;
  _thread.join();
}
void SimpleTimer::run() {
  while (_running) {
    spdlog::trace("SimpleTimer :: Checking interval");
    for (auto &&item: _waitingList) {
      if (!item.second->started)
        continue;
      item.second->timeout -= 1;
      if (0 == item.second->timeout) {
        std::unique_lock lock(_mutex);
        spdlog::trace("SimpleTimer :: Timeout occured for id: {}", item.first);
        item.second->function();
        _waitingList.erase(item.first);
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
int SimpleTimer::insert(std::function<void()> func) {
  std::unique_lock lock(_mutex);
  std::shared_ptr<WaitingItem> item = std::make_shared<WaitingItem>();
  unsigned int id = _lastId++;
  item->started = false;
  item->function = std::move(func);
  _waitingList.insert(std::make_pair(id, item));
  spdlog::trace("SimpleTimer :: Inserted new timer with id {}", id);
  return id;
}
bool SimpleTimer::start(int id, unsigned int seconds) {
  std::unique_lock lock(_mutex);
  spdlog::trace("SimpleTimer :: Starting id {} with timeout {}", id, seconds);
  auto item = _waitingList.find(id);
  if (_waitingList.end() == item)
    return false;
  _waitingList[id]->started = true;
  _waitingList[id]->timeout = seconds;
  return true;
}
bool SimpleTimer::stop(int id) {
  std::unique_lock lock(_mutex);
  spdlog::trace("SimpleTimer :: Stopping timer {}", id);
  auto item = _waitingList.find(id);
  if (_waitingList.end() == item)
    return false;
  _waitingList[id]->started = false;
  return true;
}
