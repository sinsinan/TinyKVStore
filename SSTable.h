#ifndef SSTABLE_H
#define SSTABLE_H


#include <string>
#include <fstream>
#include <map>
#include <TableMetaInfo.h>
#include <Command.h>
#include <memory>
#include<Position.h>

class SSTable
{
private:
    std::ofstream sSTableWriteStream;
    std::ifstream sSTableReadStream;
    std::map<std::string, std::shared_ptr<IndexPosition> > sparseIndex; //{key: [offset, length]}
    std::string filePath;
    TableMetaInfo tableMetaInfo;

    void initFromFile();
    void readDataFromFile();
    void openReadAndWriteStreams();
    void initFromData(std::map<std::string, std::shared_ptr<Command> > data);
    void writePartData(std::map<std::string, std::map<std::string, std::string> > partData);
    std::string findFromCommandMap(std::map<std::string, std::string> commandMap);
public:
    SSTable(){};
    SSTable(std::string sSTableFilePath);
    SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command> > data);
    ~SSTable();
    std::string query(std::string key);
};

#endif