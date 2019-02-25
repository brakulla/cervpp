/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef CERVPP_CONNECTIONHANDLER_H
#define CERVPP_CONNECTIONHANDLER_H

#include <atomic>
#include <thread>
#include <memory>
#include <functional>

#include <brutils/signal.hpp>

#include "Listener.h"
#include "Connection.h"

class ConnectionHandler {
 public:
  ConnectionHandler();
  ~ConnectionHandler();

  /*! starts a new thread and initializes a listener socket within that newly created thread */
  void startListener(int &port);
  /*! cleans up everything including listener thread */
  void stopListener();
  /*! signal slot connector */
  void connect(std::function<void(std::shared_ptr<Connection>)> func);

 private:
  /*! worker thread function */
  void listener();

 private:
  Listener _listener;
  brutils::signal<std::shared_ptr<Connection>> _signal;

 private:
  std::atomic_bool _running;
  std::thread _thread;
};

#endif //CERVPP_CONNECTIONHANDLER_H
