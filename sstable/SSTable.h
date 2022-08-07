//
// Created by ChistaData on 06/08/22.
//
#ifndef TINYKVSTORE_SSTABLE_H
#define TINYKVSTORE_SSTABLE_H

#include <fstream>
#include <map>
#include <list>
#include "TableMetaInfo.h"
#include "Command.h"

class SSTable
{
private:
    std::ofstream sSTableWriteStream;
    std::ifstream sSTableReadStream;
    std::map<std::string, std::array<long, 2> > sparseIndex; //{key: [offset, length]}
    std::string filePath;
    TableMetaInfo tableMetaInfo;

    void initFromFile();
    void openReadAndWriteStreams();
    void initFromData(std::map<std::string, std::shared_ptr<Command> > data);
    void writePartData(std::list<nlohmann::json> *partData);
    std::string findFromCommandMap(Command*);
    std::string bufferedReader(long seekPos, long byteSize);
public:
    SSTable(std::string sSTableFilePath);
    SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command> > data);
    std::string query(std::string key);
};


#endif //TINYKVSTORE_SSTABLE_H
