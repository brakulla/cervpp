//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_THREADPOOL_H
#define CERVPP_THREADPOOL_H

#include <memory>
#include <functional>
#include <thread>

#include "Thread.h"

class ThreadPool {
 public:
  ThreadPool();

  std::shared_ptr<Thread> getNewThread(std::function<void> func);

 private:
};

#endif //CERVPP_THREADPOOL_H
