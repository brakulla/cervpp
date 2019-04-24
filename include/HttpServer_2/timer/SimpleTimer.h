//
// Created by brakulla on 3/10/19.
//

/*
 * Simple Timer class for doing multiple timer operations with one thread.
 * It has precision of one second, and the operations given to this class should not take longer than a second.
 */

#ifndef CERVPP_SIMPLETIMER_H
#define CERVPP_SIMPLETIMER_H

#include <thread>
#include <mutex>
#include <functional>
#include <atomic>
#include <map>

#include <brutils/singleton_crtp.hpp>

class SimpleTimer : public brutils::singleton_crtp<SimpleTimer>
{
public:
    SimpleTimer(token);
    ~SimpleTimer();

    int insert(std::function<void()> func);
    bool start(int id, unsigned int seconds);
    bool stop(int id);
    bool remove(int id);

private:
    std::atomic_bool _running;
    std::thread _thread;
    void run();

private:
    struct WaitingItem
    {
        unsigned int timeout;
        bool started = false;
        std::function<void()> function;
    };

private:
    std::mutex _mutex;
    unsigned int _lastId;
    std::map<int, std::shared_ptr<WaitingItem>> _waitingList;
};

#endif //CERVPP_SIMPLETIMER_H
