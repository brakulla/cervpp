//
// Created by brakulla on 4/22/19.
//

#include "ParserThread.h"

ParserThread::ParserThread() :
    br_threaded_object(),
    newConnectionSlot(this),
    newRequestSignal(this),
    _requestReceivedSlot(this)
{
    newConnectionSlot.setSlotFunction(
        std::bind(&ParserThread::newConnectionReceived, this, std::placeholders::_1));
    _requestReceivedSlot.setSlotFunction([&] (std::shared_ptr<HttpRequest> request, std::shared_ptr<TcpSocket> conn) {
        newRequestSignal.emit(std::move(request), std::move(conn));
    });
}

void ParserThread::newConnectionReceived(std::shared_ptr<TcpSocket> connection)
{
    printf("New connection received\n");
    _parserList.emplace_back(std::make_unique<RequestParser>(connection, this));
    _parserList.back()->newRequestSignal.connect(_requestReceivedSlot);
    _parserList.back()->checkIncomingData();
}

