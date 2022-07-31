#ifndef KVSTORE_COMMAND_H
#define KVSTORE_COMMAND_H

#include <string>
#include <map>

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
    Command(){};
    CommandType getCommandType() { return this->commandType; };
    virtual std::map<std::string, std::string> getMapOfCommand() = 0;
    virtual std::string getKey() = 0;
    ~Command(){};
};

class SetCommand : public Command
{
private:
    CommandType commandType = SET;
    std::string key;
    std::string value;

public:
    SetCommand(std::string key, std::string value)
    {
        this->key = key;
        this->value = value;
    }
    std::string getKey() { return this->key; }
    std::string getValue() { return this->value; }
    ~SetCommand(){};
    std::map<std::string, std::string> getMapOfCommand()
    {
        printf("inside getMapOfCommand of SetCommand\n");
        std::map<std::string, std::string> mapOfCommand;
        std::pair<std::string, std::string> currentPair;

        currentPair.first = "key";
        currentPair.second = this->key;
        mapOfCommand.insert(currentPair);

        currentPair.first = "value";
        currentPair.second = this->value;
        mapOfCommand.insert(currentPair);

        currentPair.first = "commandType";
        currentPair.second = this->commandType;
        mapOfCommand.insert(currentPair);
        return mapOfCommand;
    }
};

class RmCommand : public Command
{
private:
    CommandType commandType = RM;
    std::string key;

public:
    RmCommand(std::string key)
    {
        this->key = key;
    }
    std::string getKey() { return this->key; }
    ~RmCommand(){};
    std::map<std::string, std::string> getMapOfCommand()
    {
        printf("inside getMapOfCommand of RmCommand\n");
        std::map<std::string, std::string> mapOfCommand;
        std::pair<std::string, std::string> currentPair;

        currentPair.first = "key";
        currentPair.second = this->key;
        mapOfCommand.insert(currentPair);

        currentPair.first = "commandType";
        currentPair.second = this->commandType;
        mapOfCommand.insert(currentPair);
        return mapOfCommand;
    }
};

#endif