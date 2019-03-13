/**
 * @file ResourceController.cpp
 *
 * Description
 */

#include <controller/ResourceController.h>

#include "ResourceController.h"

void ResourceController::process(std::shared_ptr<HttpRequest> req, std::shared_ptr<HttpResponse> resp) {
  std::vector<std::string> pathLevels;
  brutils::split_string(req->getURI(), pathLevels, '/');
  if (!pathLevels.empty()) {
    _resourceMap[pathLevels.at(pathLevels.size()-1)](req, resp);
  }
}
void ResourceController::addResource(std::string resource,
                                     std::function<void(std::shared_ptr<HttpRequest>,
                                                        std::shared_ptr<HttpResponse>)> callback) {
  _resourceMap.insert(std::make_pair(resource, callback));
}
