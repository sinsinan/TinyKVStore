//
// Created by ChistaData on 06/08/22.
//

#ifndef TINYKVSTORE_SETCOMMAND_H
#define TINYKVSTORE_SETCOMMAND_H


#include "Command.h"

class SetCommand : public Command
{
private:
    CommandType commandType = SET;
    std::string key;
    std::string value;

public:
    SetCommand(std::string key, std::string value);
    SetCommand(nlohmann::json j);
    std::string getKey();
    std::string getValue();
    nlohmann::json toJson();
    CommandType getCommandType();
};


#endif //TINYKVSTORE_SETCOMMAND_H
