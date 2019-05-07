//
// Created by brakulla on 09.10.2018.
//

#include "DefaultController.h"

DefaultController::DefaultController()
{

}

DefaultController::~DefaultController()
{

}

void DefaultController::ProcessRequest(HttpRequest &request)
{
    HttpResponse response(request);
    response.status(404);
    response.setHeader("Connection", "keep-alive");
    response.send();
}
