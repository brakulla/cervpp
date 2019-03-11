/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

/*
 * TCP/IP level server class.
 */

#ifndef CERVPP_CONNECTIONHANDLER_H
#define CERVPP_CONNECTIONHANDLER_H

#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <condition_variable>
#include <errno.h>
#include <atomic>
#include <cstring>
#include <poll.h>
#include <fcntl.h>

#include <brutils/signal.hpp>

#include "Configuration.h"
#include "Connection.h"
#include "RequestParser.h"

class ConnectionHandler {
 public:
  ConnectionHandler();
  ~ConnectionHandler();

  void start(unsigned long port, int maxConnectionSize);
  void stop();
  void waitForFinished();

  void pushIdleSocket(std::shared_ptr<Connection> connection);

  void registerNewRequestReceived(std::function<void(std::shared_ptr<Connection>, std::shared_ptr<HttpRequest>)> func);

 private:
  /*! worker thread function */
  void run();
  void clearTimeoutSockets(struct pollfd* socketList, int &socketListSize);
  void processSockets(struct pollfd* socketList, int &socketListSize);
  void parseIncomingData(int sockedFd);

 private:
  brutils::signal<std::shared_ptr<Connection>, std::shared_ptr<HttpRequest>> _signal;

 private: // conf
  int _maxConnSize;
 private:
  int _serverFd;
  int _socketListSize;
  struct pollfd *_socketList;
  struct sockaddr_in _serverAddr;
  std::atomic_bool _running;
  std::thread _thread;
  std::map<int, std::shared_ptr<RequestParser>> _parserMap;
  std::map<int, int> _timeoutMap;
};

#endif //CERVPP_CONNECTIONHANDLER_H
