//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_TCPLISTENER_H
#define CERVPP_TCPLISTENER_H

#include <condition_variable>
#include <queue>
#include <thread>
#include <atomic>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <brutils/queue_pc.h>

class Listener {
 public:
  Listener() = default;
  ~Listener() = default;

  /*! initializes a listener socket */
  void init(int &port);
  /*! starts listening socket in a new thread */
  void start();
  /*! command to stop listener thread */
  void stop();
  /*! waits for listener thread to stop */
  void waitForFinish();
  /*! blocks until next connection is available and returns the next socket descriptor */
  int getNextConnection();

 private:
  void run();

 private:
  std::atomic_int _running;
  std::thread _listenerThread;

 private:
  int _serverFd;
  struct sockaddr_in _serverAddr;

 private:
  brutils::queue_pc<int> _queue;
};

#endif //CERVPP_TCPLISTENER_H
