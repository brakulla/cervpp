//
// Created by brakulla on 4/22/19.
//

#ifndef PARSERTHREAD_H
#define PARSERTHREAD_H

#include <memory>

#include <brutils/br_object.hpp>

#include "RequestParser.h"
#include "TcpSocket.h"

/*!
 * @class Single thread, parser container. Used to manage parser objects in a separate thread.
 */
class ParserThread : public brutils::br_threaded_object
{
public:
    ParserThread();

    brutils::slot<std::shared_ptr<TcpSocket>> newConnectionSlot;
    brutils::signal<std::shared_ptr<HttpRequest>, std::shared_ptr<TcpSocket>> newRequestSignal;

private:
    brutils::slot<std::shared_ptr<HttpRequest>, std::shared_ptr<TcpSocket>> _requestReceivedSlot;
    void newConnectionReceived(std::shared_ptr<TcpSocket> connection);
    std::vector<std::unique_ptr<RequestParser>> _parserList;
};

#endif //PARSERTHREAD_H
