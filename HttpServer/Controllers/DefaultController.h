//
// Created by brakulla on 09.10.2018.
//

#ifndef CERVPP_DEFAULTCONTROLLER_H
#define CERVPP_DEFAULTCONTROLLER_H

#include "IController.h"

class DefaultController : public IController
{
public:
    DefaultController();
    ~DefaultController() override;

    void NewRequestReceived(HttpRequest &request) override;
};


#endif //CERVPP_DEFAULTCONTROLLER_H
