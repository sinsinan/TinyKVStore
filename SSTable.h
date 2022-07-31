#ifndef SSTABLE_H
#define SSTABLE_H


#include <string>
#include <fstream>
#include <map>
#include <TableMetaInfo.h>
#include <Command.h>
#include <memory>

class SSTable
{
private:
    // std::streampos sSTableFileStreamBottom;
    // std::streampos sSTableFileStreamTop;
    // long long fileBaseOffset;
    // std::fstream sSTableFileStream;
    std::ofstream sSTableWriteStream;
    std::ifstream sSTableReadStream;
    std::map<std::string, std::pair<long, long>> sparseIndex; //{key: [position, length]}
    std::string filePath;
    TableMetaInfo tableMetaInfo;

    void initFromFile();
    void readDataFromFile();
    void openReadAndWriteStreams();
    void initFromData(std::map<std::string, std::shared_ptr<Command>> data);
public:
    SSTable(){};
    SSTable(std::string sSTableFilePath);
    SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command>> data);
    ~SSTable();
};

#endif