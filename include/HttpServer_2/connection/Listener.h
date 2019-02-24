//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_TCPLISTENER_H
#define CERVPP_TCPLISTENER_H

#include <condition_variable>

class Listener
{
public:
    Listener() = default;
    ~Listener() = default;

    void Init();
    void Listen();

    int GetNextConnection();
private:
    std::mutex _mutex;
    std::condition_variable _condVar;
};

#endif //CERVPP_TCPLISTENER_H
