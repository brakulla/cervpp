//
// Created by brakulla on 3/10/19.
//

#include <timer/SimpleTimer.h>

#include "timer/SimpleTimer.h"
SimpleTimer::SimpleTimer()
    : _running(true), _lastId(0)
{
    printf("SimpleTimer :: Starting timer\n");
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
        printf("SimpleTimer :: Checking interval\n");
        for (auto &&item: _waitingList) {
            if (!item.second->started)
                continue;
            item.second->timeout -= 1;
            if (0 == item.second->timeout) {
                std::unique_lock lock(_mutex);
                printf("SimpleTimer :: Timeout occured for id: %d\n", item.first);
                item.second->function();
                _waitingList.erase(item.first);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
int SimpleTimer::insert(std::function<void()> func)
{
    std::unique_lock lock(_mutex);
    std::shared_ptr<WaitingItem> item = std::make_shared<WaitingItem>();
    unsigned int id = _lastId++;
    item->started = false;
    item->function = std::move(func);
    _waitingList.insert(std::make_pair(id, item));
    printf("SimpleTimer :: Inserted new timer with id %d\n", id);
    return id;
}
bool SimpleTimer::start(int id, unsigned int seconds)
{
    std::unique_lock lock(_mutex);
    printf("SimpleTimer :: Starting id %d with timeout %d\n", id, seconds);
    auto item = _waitingList.find(id);
    if (_waitingList.end() == item)
        return false;
    _waitingList[id]->started = true;
    _waitingList[id]->timeout = seconds;
    return true;
}
bool SimpleTimer::stop(int id)
{
    std::unique_lock lock(_mutex);
    printf("SimpleTimer :: Stopping timer %d\n", id);
    auto item = _waitingList.find(id);
    if (_waitingList.end() == item)
        return false;
    _waitingList[id]->started = false;
    return true;
}

bool SimpleTimer::remove(int id)
{
    bool found = stop(id);
    if (found)
        _waitingList.erase(id);
    return found;
}
