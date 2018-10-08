//
// Created by brakulla on 09.10.2018.
//

#ifndef CERVPP_ICONTROLLER_H
#define CERVPP_ICONTROLLER_H

#include "../HttpRequest.h"
#include "../HttpResponse.h"

enum class ControllerType {
    eSTATICFILE_CONTROLLER,
    eDYNAMIC_CONTROLLER
};

class IController
{
public:
    IController();
    virtual ~IController();

    virtual void NewRequestReceived(HttpRequest &request) = 0;

private:
};


#endif //CERVPP_ICONTROLLER_H
