//
// Created by brakulla on 3/10/19.
//

#include "SimpleTimer.h"

SimpleTimer::SimpleTimer(token)
    : timeout(nullptr), _running(true), _lastId(0)
{
    _thread = std::thread(&SimpleTimer::run, this);
}

SimpleTimer::~SimpleTimer()
{
    _running = false;
    _thread.join();
}

void SimpleTimer::run()
{
    while (_running) {
        for (auto &&item: _waitingList) {
            if (!item.second->started)
                continue;
            item.second->timeout -= 1;
            if (0 == item.second->timeout) {
                std::unique_lock lock(_mutex);
                timeout.emit(item.first);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int SimpleTimer::start(unsigned int seconds)
{
    std::unique_lock lock(_mutex);
    auto item = std::make_shared<WaitingItem>();
    unsigned int id = _lastId++;
    item->started = true;
    item->timeout = seconds;
    _waitingList[id] = item;
    return id;
}

bool SimpleTimer::restart(int id, unsigned int seconds)
{
    std::unique_lock lock(_mutex);
    auto item = _waitingList.find(id);
    if (_waitingList.end() == item)
        return false;
    item->second->started = true;
    item->second->timeout = seconds;
    return true;
}

bool SimpleTimer::stop(int id)
{
    std::unique_lock lock(_mutex);
    auto item = _waitingList.find(id);
    if (_waitingList.end() == item)
        return false;
    item->second->started = false;
    return true;
}

bool SimpleTimer::remove(int id)
{
    bool found = stop(id);
    if (found)
        _waitingList.erase(id);
    return found;
}
