//
// Created by ChistaData on 06/08/22.
//
#include "RmCommand.h"

RmCommand::RmCommand(std::string key)
{
this->key = key;
}
RmCommand::RmCommand(nlohmann::json j)
{
this->key = j["key"].get<std::string>();
}
std::string RmCommand::getKey() { return this->key; }
nlohmann::json RmCommand::toJson()
{
//    printf("inside toJson of RmCommand\n");
    nlohmann::json j;

    j["key"] = this->key;
    j["commandType"] = this->commandType;

    return j;
}
std::string RmCommand::getValue() { return ""; }

CommandType RmCommand::getCommandType() {return commandType;}