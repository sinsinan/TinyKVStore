//
// Created by ChistaData on 06/08/22.
//

#ifndef TINYKVSTORE_RMCOMMAND_H
#define TINYKVSTORE_RMCOMMAND_H

#include "Command.h"

class RmCommand : public Command
{
private:
    CommandType commandType = RM;
    std::string key;

public:
    RmCommand(std::string key);
    RmCommand(nlohmann::json j);
    std::string getKey();
    nlohmann::json toJson();
    std::string getValue();
    CommandType getCommandType();
};

#endif //TINYKVSTORE_RMCOMMAND_H
