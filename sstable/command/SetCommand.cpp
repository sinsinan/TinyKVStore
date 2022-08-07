//
// Created by ChistaData on 06/08/22.
//
#include "SetCommand.h"
SetCommand::SetCommand(std::string key, std::string value)
{
this->key = key;
this->value = value;
}
SetCommand::SetCommand(nlohmann::json j)
{
this->key = j["key"].get<std::string>();
this->value = j["value"].get<std::string>();
}
std::string SetCommand::getKey() { return this->key; }
std::string SetCommand::getValue() { return this->value; }
nlohmann::json SetCommand::toJson()
{
    printf("inside toJson of SetCommand\n");
    nlohmann::json j;

    j["key"] = this->key;
    j["value"] = this->value;
    j["commandType"] = this->commandType;

    return j;
}

CommandType SetCommand::getCommandType() {return commandType;}