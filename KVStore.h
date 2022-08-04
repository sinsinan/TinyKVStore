

#ifndef KVSTORE_H
#define KVSTORE_H

#include <fstream>
#include <map>
#include <list>
#include "json.h"

class TableMetaInfo
{
private:
    long version;
    long dataStart;
    long dataLength;
    long indexStart;
    long indexLength;
    long partSize;
public:
    TableMetaInfo(){};
    void readFromFile(std::ifstream &file);
    void writeToFile(std::ofstream &file);
    ~TableMetaInfo(){};
    long getVersion();
    long getDataStart();
    long getDataLength();
    long getIndexStart();
    long getIndexLength();
    long getPartSize();
    void setVersion(long version);
    void setDataStart(long dataStart);
    void setDataLength(long dataLength);
    void setIndexStart(long indexStart);
    void setIndexLength(long indexLength);
    void setPartSize(long partSize);
};

//class IndexPosition
//{
//private:
//    long offset;
//    long length;

//public:
//    IndexPosition(long offset, long length);
//    long getOffset();
//    long getLength();
//    ~IndexPosition();
//};

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
    Command(nlohmann::json){};
    virtual nlohmann::json toJson() = 0;
    virtual std::string getKey() = 0;
    virtual std::string getValue() = 0;
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
    SetCommand(nlohmann::json j)
    {
        this->key = j["key"].get<std::string>();
        this->value = j["value"].get<std::string>();
    }
    std::string getKey() { return this->key; }
    std::string getValue() { return this->value; }
    nlohmann::json toJson()
    {
        printf("inside toJson of SetCommand\n");
        nlohmann::json j;

        j["key"] = this->key;
        j["value"] = this->value;
        j["commandType"] = this->commandType;

        return j;
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
    RmCommand(nlohmann::json j)
    {
        this->key = j["key"].get<std::string>();
    }
    std::string getKey() { return this->key; }
    nlohmann::json toJson()
    {
        printf("inside toJson of RmCommand\n");
        nlohmann::json j;

        j["key"] = this->key;
        j["commandType"] = this->commandType;

        return j;
    }
    std::string getValue() { return ""; }
};

class SSTable
{
private:
    std::ofstream sSTableWriteStream;
    std::ifstream sSTableReadStream;
    std::map<std::string, std::array<long, 2> > sparseIndex; //{key: [offset, length]}
    std::string filePath;
    TableMetaInfo tableMetaInfo;

    void initFromFile();
    void readDataFromFile();
    void openReadAndWriteStreams();
    void initFromData(std::map<std::string, std::shared_ptr<Command> > data);
    void writePartData(std::map<std::string, std::map<std::string, std::string> > partData);
    std::string findFromCommandMap(Command*);
    std::string bufferedReader(long seekPos, long byteSize);
    std::shared_ptr<Command>  convertToCommand(nlohmann::json j);
public:
    SSTable(std::string sSTableFilePath);
    SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command> > data);
    std::string query(std::string key);
//    ~SSTable();
};

#endif
