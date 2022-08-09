//
// Created by ChistaData on 06/08/22.
//

#ifndef TINYKVSTORE_COMMANDUTIL_H
#define TINYKVSTORE_COMMANDUTIL_H

#include "SetCommand.h"
#include "RmCommand.h"

std::shared_ptr<Command>  convertToCommand(nlohmann::json j);

#endif //TINYKVSTORE_COMMANDUTIL_H
