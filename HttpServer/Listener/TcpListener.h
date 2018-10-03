/*
 * Created on 03.10.2018 by Burak Ongay
 *
 * @brief Incoming TCp conenction listener
 * @description
 *  This class is creates a listener socket and uses HttRequestHandler to parse the incoming data
 *  If incoming data comply with HTTP standards, then this class will send the data to main thread through callback
 *      function.
 */

#ifndef CERVPP_TCPLISTENER_H
#define CERVPP_TCPLISTENER_H


#include <functional>

class TcpListener {
public:
    // TODO: change the signature ot below function to call the callback function with HttpRequest parameter
    void Listen(int port, std::function<void(void)>callback);
};


#endif //CERVPP_TCPLISTENER_H
