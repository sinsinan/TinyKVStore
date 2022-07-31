#include <SSTable.h>
#include <string>
#include <TableMetaInfo.h>
#include <Command.h>
#include <iterator>
#include <list>
#include <vector>
#include <json.hpp>
#include <iostream>
#include <fstream>

SSTable::SSTable(std::string sSTableFilePath)
{
    this->filePath = sSTableFilePath;
    this->openReadAndWriteStreams();
    this->initFromFile();
}

SSTable::SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command>> data)
{
    this->filePath = sSTableFilePath;
    this->tableMetaInfo.setPartSize(partSize);
    this->openReadAndWriteStreams();
    this->initFromData(data);
}

SSTable::~SSTable()
{
    printf("SSTable destructor called\n");
    this->sSTableReadStream.close();
    this->sSTableWriteStream.close();
    // this->sSTableFileStream.close();
}

void SSTable::initFromFile()
{
    // this->tableMetaInfo.readFromFile(this->sSTableFileStream);
    // printf("(long long)(this->sSTableFileStreamBottom) + this->tableMetaInfo.getIndexStart() : %d\n", (long long)(this->sSTableFileStreamBottom) + this->tableMetaInfo.getIndexStart());
    // this->sSTableFileStream.seekg(0, std::ios::end);
    // this->sSTableFileStream.seekg((long long)(this->sSTableFileStream.tellg()) - (4 * 6) - this->tableMetaInfo.getIndexLength());
    // this->sSTableFileStream.read((char *)&this->sparseIndex, this->tableMetaInfo.getIndexLength());
    // printf("this->sparseIndex: %d\n", this->sparseIndex.size());
    // for (auto itr = this->sparseIndex.begin(); itr != this->sparseIndex.end(); ++itr)
    // {
    //     printf("key: %s\n", itr->first);
    // }

    this->tableMetaInfo.readFromFile(this->sSTableReadStream);
    this->sSTableReadStream.seekg(0, std::ios::end);
    this->sSTableReadStream.seekg((long long)(this->sSTableReadStream.tellg()) - (4 * 6) - this->tableMetaInfo.getIndexLength());
    std::vector<char> vec(this->tableMetaInfo.getIndexLength());
    this->sSTableReadStream.read(reinterpret_cast<char *>(vec.data()), this->tableMetaInfo.getIndexLength());

    auto sparseIndexString = std::string(vec.begin(), vec.end()).c_str();

    printf("sparseIndexString: %s\n", sparseIndexString);
    // printf("this->sparseIndex: %d\n", this->sparseIndex.size());
    // for (auto itr = this->sparseIndex.begin(); itr != this->sparseIndex.end(); ++itr)
    // {
    //     printf("key: %s\n", itr->first);
    // }
}

void SSTable::openReadAndWriteStreams()
{
    // this->sSTableFileStream.open(this->filePath, std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
    // if (!this->sSTableFileStream)
    // {
    //     printf("\n!this->sSTableFileStream failed\n");
    //     throw "this->sSTableFileStream failed";
    // }
    // printf("\nopened sSTableFileStream\n");

    this->sSTableWriteStream.open(this->filePath, std::ios::binary | std::ios::out | std::ios::app);
    if (!this->sSTableWriteStream)
    {
        printf("\n!this->sSTableWriteStream failed\n");
        throw "this->sSTableWriteStream failed";
    }
    printf("\nopened sSTableWriteStream\n");
    // sSTableFileStream.seekg(0, std::ios::beg);
    // this->sSTableFileStreamBottom = sSTableFileStream.tellg();
    // sSTableFileStream.seekg(0, std::ios::end);
    // this->sSTableFileStreamTop = sSTableFileStream.tellg();
    // this->fileBaseOffset = this->sSTableFileStreamTop - this->sSTableFileStreamBottom;
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
    printf("find sinan1 :%s\n", data.find("sinan1")->second->getKey());
    for (auto itr = data.begin(); itr != data.end(); ++itr)
    {
        printf("iterating keys...\n");
        printf("key: %s\n", itr->first.c_str());
    }
    // this->tableMetaInfo.setDataStart(this->sSTableFileStream.tellp() - this->sSTableFileStreamBottom);
    this->tableMetaInfo.setDataStart(this->sSTableWriteStream.tellp());
    long currentPartSize = 0;
    std::map<std::string, std::map<std::string, std::string>> partDataMap;
    // std::list<std::shared_ptr<Command>> partDataList;
    for (auto itr = data.begin(); itr != data.end(); ++itr)
    {
        printf("iterating data...\n");
        printf("key: %s, value: %s\n", itr->first, (*itr->second).getMapOfCommand());
        partDataMap.insert(std::make_pair(itr->first, (*itr->second).getMapOfCommand()));
        nlohmann::json partDataMapFirst(partDataMap.begin()->first);
        printf("partDataMapFirst: %s\n", partDataMap.begin()->first);
        // partDataList.push_back(itr->second);
        currentPartSize++;
        if (currentPartSize >= this->tableMetaInfo.getPartSize())
        {
            nlohmann::json partDataListJson(partDataMap);
            auto partDataListJsonStr = partDataListJson.dump().c_str();
            printf("partDataListJsonStr: %s\n", partDataListJsonStr);
            printf("strlen(partDataListJsonStr): %d\n", strlen(partDataListJsonStr));
            this->sparseIndex.insert(std::make_pair(partDataMap.begin()->first.c_str(), std::make_pair(this->sSTableWriteStream.tellp(), strlen(partDataListJsonStr))));
            this->sSTableWriteStream.write(partDataListJsonStr, strlen(partDataListJsonStr));

            // this->sparseIndex.insert(std::make_pair(partDataList.front()->getKey(), std::make_pair(this->sSTableFileStream.tellp() - this->sSTableFileStreamBottom, sizeof(partDataList))));
            // this->sSTableFileStream.write((char *)&partDataList, sizeof(partDataList));

            currentPartSize = 0;
            partDataMap.clear();
            // partDataList.clear();
        }
    }

    printf("currentPartSize: %d\n", currentPartSize);
    printf("partDataMapsize: %d\n", partDataMap.size());
    if (currentPartSize > 0)
    {
        nlohmann::json partDataListJson(partDataMap);
        auto partDataListJsonStr = partDataListJson.dump().c_str();
        printf("partDataListJsonStr: %s\n", partDataListJsonStr);
        printf("strlen(partDataListJsonStr): %d\n", strlen(partDataListJsonStr));
        this->sparseIndex.insert(std::make_pair(partDataMap.begin()->first.c_str(), std::make_pair(this->sSTableWriteStream.tellp(), strlen(partDataListJsonStr))));
        this->sSTableWriteStream.write(partDataListJsonStr, strlen(partDataListJsonStr));

        // this->sparseIndex.insert(std::make_pair(partDataList.front()->getKey(), std::make_pair(this->sSTableFileStream.tellp() - this->sSTableFileStreamBottom, sizeof(partDataList))));
        // this->sSTableFileStream.write((char *)&partDataList, sizeof(partDataList));

        currentPartSize = 0;
        partDataMap.clear();
        // partDataList.clear();
    }

    // this->tableMetaInfo.setDataLength((long)this->sSTableFileStream.tellp() - this->sSTableFileStreamBottom);
    this->tableMetaInfo.setDataLength((long long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart());

    // this->tableMetaInfo.setIndexStart(this->sSTableFileStream.tellp() - this->sSTableFileStreamBottom);
    this->tableMetaInfo.setIndexStart((long long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart());

    printf("this->sparseIndex: %d\n", this->sparseIndex.size());
    for (auto itr = this->sparseIndex.begin(); itr != this->sparseIndex.end(); ++itr)
    {
        printf("key: %s\n", itr->first.c_str());
    }

    nlohmann::json sparseIndexajson(sparseIndex);
    auto sparseIndexCharArray = sparseIndexajson.dump().c_str();
    printf("sparseIndexCharArray: %s\n", sparseIndexCharArray);
    printf("strlen(sparseIndexCharArray): %d\n", strlen(sparseIndexCharArray));
    this->sSTableWriteStream.write(sparseIndexCharArray, strlen(sparseIndexCharArray));
    this->tableMetaInfo.setIndexLength(strlen(sparseIndexCharArray));
    // this->sSTableFileStream.write((char *)&this->sparseIndex, sizeof(sparseIndex));
    // this->tableMetaInfo.setIndexLength(sizeof(this->sparseIndex));

    // this->tableMetaInfo.writeToFile(this->sSTableFileStream);
    this->tableMetaInfo.writeToFile(this->sSTableWriteStream);
}