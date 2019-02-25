//
// Created by brakulla on 18.02.2019.
//

#ifndef CERVPP_CONTROLLER_H
#define CERVPP_CONTROLLER_H

class IController
{
private:
    enum ControllerType {
        StaticFileController,   // returns the file named requested by the path
        ResourceController,     // processes the request in a function per resource manner
        RestfulAPIController    // processes the request in a function per http request type manner
    };
};

#endif //CERVPP_CONTROLLER_H
