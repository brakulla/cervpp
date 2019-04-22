/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_THREAD_H
#define CERVPP_THREAD_H

#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

#include <brutils/queue_pc.hpp>

class ServerThread
{
public:
    ServerThread();
    ServerThread(const ServerThread &) = delete;
    ~ServerThread();

    void start();
    void stop();
    bool isExecuting();
    void execute(std::function<void()> func);

private:
    std::atomic_bool _running;
    std::atomic_bool _executing;
    std::mutex _mutex;
    std::thread _thread;

private:
    void run();

private:
    brutils::queue_pc<std::function<void()>> _queue;

};

#endif //CERVPP_THREAD_H
