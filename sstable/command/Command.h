//
// Created by ChistaData on 06/08/22.
//

#ifndef TINYKVSTORE_COMMAND_H
#define TINYKVSTORE_COMMAND_H

#include "json.h"
#include<string>

enum CommandType
{
    GET,
    SET,
    RM
};

class Command
{
private:
    CommandType commandType;
public:
    virtual CommandType getCommandType() = 0;
    virtual nlohmann::json toJson() = 0;
    virtual std::string getKey() = 0;
    virtual std::string getValue() = 0;
};

#endif //TINYKVSTORE_COMMAND_H
