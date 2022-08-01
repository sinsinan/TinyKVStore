#include <SSTable.h>
#include <string>
#include <TableMetaInfo.h>
#include <Command.h>
#include <iterator>
#include <list>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

SSTable::SSTable(std::string sSTableFilePath)
{
    this->filePath = sSTableFilePath;
    this->openReadAndWriteStreams();
    this->initFromFile();
    printf("sparseIndex.size(): %s\n", sparseIndex.size());
}

SSTable::SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command>> data)
{
    try
    {
        this->filePath = sSTableFilePath;
        this->tableMetaInfo.setPartSize(partSize);
        this->openReadAndWriteStreams();
        this->initFromData(data);
        printf("sparseIndex.size(): %s\n", sparseIndex.size());
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

SSTable::~SSTable()
{
    printf("SSTable destructor called\n");
    this->sSTableReadStream.close();
    this->sSTableWriteStream.close();
}

void SSTable::initFromFile()
{
    this->tableMetaInfo.readFromFile(this->sSTableReadStream);
    this->sSTableReadStream.seekg(0, std::ios::end);
    this->sSTableReadStream.seekg((long long)(this->sSTableReadStream.tellg()) - (4 * 6) - this->tableMetaInfo.getIndexLength());
    this->sSTableReadStream.read((char *)&sparseIndex, this->tableMetaInfo.getIndexLength());
}

void SSTable::openReadAndWriteStreams()
{

    this->sSTableWriteStream.open(this->filePath, std::ios::binary | std::ios::out | std::ios::app);
    if (!this->sSTableWriteStream)
    {
        printf("\n!this->sSTableWriteStream failed\n");
        throw "this->sSTableWriteStream failed";
    }
    printf("\nopened sSTableWriteStream\n");
    this->sSTableReadStream.open(this->filePath, std::ios::binary | std::ios::app);
    if (!this->sSTableReadStream)
    {
        printf("\nthis->sSTableReadStream.open failed\n");
        throw "this->sSTableReadStream.open failed";
    }
    printf("\nopened SSTableReadStream\n");
}

void SSTable::initFromData(std::map<std::string, std::shared_ptr<Command>> data)
{
    printf("Loading table from index\n");
    printf("total data -> %d\n", data.size());
    this->tableMetaInfo.setDataStart(this->sSTableWriteStream.tellp());
    long currentPartSize = 0;
    std::list<std::map<std::string, std::string>> partDataMap;
    for (auto itr = data.begin(); itr != data.end(); ++itr)
    {
        printf("iterating data...\n");
        printf("key: %s\n", (itr->first).c_str());
        partDataMap.push_back((*itr->second).getMapOfCommand());
        currentPartSize++;
        if (currentPartSize >= this->tableMetaInfo.getPartSize())
        {
                this->sparseIndex[partDataMap.begin()->find("key")->second] = std::make_shared<IndexPosition>(IndexPosition( (long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart(), sizeof(partDataMap)));
            // this->sparseIndex.insert(std::make_pair(partDataMap.begin()->first, std::make_pair(((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()), sizeof(partDataMap))));
            printf("partDataMap.begin()->first : %s\n", partDataMap.begin()->find("key")->second.c_str());
            printf("((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()) : %d\n", ((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()));
            printf("((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()) : %d\n", ((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()));
            printf("sizeof(partDataMap) : %d\n", sizeof(partDataMap));
            printf("sparseIndexLast key: %s, offset: %d, length: %d\n", sparseIndex.end()->first.c_str(), (*sparseIndex.end()->second).getOffset(), (*sparseIndex.end()->second).getLength());
            this->sSTableWriteStream.write((char *)&partDataMap, sizeof(partDataMap));

            currentPartSize = 0;
            partDataMap.clear();
        }
    }
    if (currentPartSize > 0)
    {

            this->sparseIndex[partDataMap.begin()->find("key")->second] = std::make_shared<IndexPosition>(IndexPosition( (long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart(), sizeof(partDataMap)));
            // this->sparseIndex.insert(std::make_pair(partDataMap.begin()->first, std::make_pair(((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()), sizeof(partDataMap))));
            printf("partDataMap.begin()->first : %s\n", partDataMap.begin()->find("key")->second.c_str());
            printf("((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()) : %d\n", ((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()));
            printf("((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()) : %d\n", ((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart()));
            printf("sizeof(partDataMap) : %d\n", sizeof(partDataMap));
            printf("sparseIndexLast key: %s, offset: %d, length: %d\n", sparseIndex.end()->first.c_str(), (*sparseIndex.end()->second).getOffset(), (*sparseIndex.end()->second).getLength());
            this->sSTableWriteStream.write((char *)&partDataMap, sizeof(partDataMap));

            currentPartSize = 0;
            partDataMap.clear();
    }

    this->tableMetaInfo.setDataLength((long long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart());

    this->tableMetaInfo.setIndexStart((long long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart());

    this->sSTableWriteStream.write((char *)&sparseIndex, sizeof(sparseIndex));
    this->tableMetaInfo.setIndexLength(sizeof(sparseIndex));
    this->tableMetaInfo.writeToFile(this->sSTableWriteStream);
}

void SSTable::writePartData(std::map<std::string, std::map<std::string, std::string>> partData)
{
    // this->sparseIndex.insert(std::make_pair(partData.begin()->first, std::make_pair((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart(), sizeof(partData))));
    // this->sSTableWriteStream.write((char *)&partData, sizeof(partData));
}

std::string SSTable::query(std::string key)
{
    printf("Starting to query key:%s\n", key.c_str());
    std::shared_ptr<IndexPosition> firstKeyPosition = NULL;
    std::shared_ptr<IndexPosition> lastKeyPosition = NULL;
    for (auto itr = sparseIndex.begin(); itr != sparseIndex.end(); ++itr)
    {
        printf("searchkey: %s, itrkey: %s, cmp: %d\n", key.c_str(), itr->first.c_str(), itr->first.compare(key));
        if (itr->first.compare(key) <= 0)
        {
            firstKeyPosition = itr->second;
        }
        else
        {
            lastKeyPosition = itr->second;
            break;
        }
    }

    if (firstKeyPosition != NULL)
    {
        std::map<std::string, std::map<std::string, std::string>> dataMap;
        this->sSTableReadStream.seekg(this->tableMetaInfo.getDataStart() + firstKeyPosition->getOffset());
        this->sSTableReadStream.read((char *)&dataMap, firstKeyPosition->getLength());
        auto dataMapPair = dataMap.find(key);
        if (dataMapPair != dataMap.end())
        {
            return findFromCommandMap(dataMapPair->second);
        }
    }

        if (lastKeyPosition != NULL)
    {
        std::map<std::string, std::map<std::string, std::string>> dataMap;
        this->sSTableReadStream.seekg(this->tableMetaInfo.getDataStart() + lastKeyPosition->getOffset());
        this->sSTableReadStream.read((char *)&dataMap, lastKeyPosition->getLength());
        auto dataMapPair = dataMap.find(key);
        if (dataMapPair != dataMap.end())
        {
            return findFromCommandMap(dataMapPair->second);
        }
    }

    return NULL;
}

std::string SSTable::findFromCommandMap(std::map<std::string, std::string> commandMap)
{
    if (commandMap.find("value") != commandMap.end())
    {
        return commandMap.find("value")->second;
    }
    return NULL;
}