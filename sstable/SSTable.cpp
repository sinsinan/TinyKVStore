//
// Created by ChistaData on 06/08/22.
//
#include<iostream>

#include "SSTable.h"
#include "CommandUtil.h"

SSTable::SSTable(std::string sSTableFilePath)
{
    this->filePath = sSTableFilePath;
    this->openReadAndWriteStreams();
    this->initFromFile();
}

SSTable::SSTable(std::string sSTableFilePath, long partSize, std::map<std::string, std::shared_ptr<Command> > data)
{
    this->filePath = sSTableFilePath;
    this->tableMetaInfo.setPartSize(partSize);
    this->openReadAndWriteStreams();
    this->initFromData(data);

}

void SSTable::initFromFile()
{
    this->tableMetaInfo.readFromFile(this->sSTableReadStream);
    std::string jsonString = this->bufferedReader(this->tableMetaInfo.getIndexStart(), this->tableMetaInfo.getIndexLength());
    nlohmann::json sparseIndexJson = nlohmann::json::parse(jsonString);

    for (nlohmann::json::iterator it = sparseIndexJson.begin(); it != sparseIndexJson.end(); ++it) {
        this->sparseIndex[it.key()] = {sparseIndexJson[it.key()][0].get<long>(), sparseIndexJson[it.key()][1].get<long>()};
    }
    std::cout<<"index json: "<<jsonString<<std::endl;
}

void SSTable::openReadAndWriteStreams()
{

    this->sSTableWriteStream.open(this->filePath, std::ios_base::binary | std::ios_base::out | std::ios_base::app);
    if (!this->sSTableWriteStream)
    {
        std::cout << ("\n!this->sSTableWriteStream failed\n");
        throw "this->sSTableWriteStream failed";
    }
    std::cout << ("\nopened sSTableWriteStream\n");
    this->sSTableReadStream.open(this->filePath, std::ios_base::binary | std::ios_base::app | std::ios_base::in);
    if (!this->sSTableReadStream)
    {
        std::cout << ("\nthis->sSTableReadStream.open failed\n");
        throw "this->sSTableReadStream.open failed";
    }
    std::cout << ("\nopened SSTableReadStream\n");
}

void SSTable::initFromData(std::map<std::string, std::shared_ptr<Command> > data)
{
    std::cout << ("Loading table from index\n");
    std::cout << "total data -> %d" << data.size() << std::endl;
    this->tableMetaInfo.setDataStart(this->sSTableWriteStream.tellp());
    long currentPartSize = 0;
    std::list<nlohmann::json> partDataMap;
    for (auto itr = data.begin(); itr != data.end(); ++itr)
    {
        partDataMap.push_back((*itr->second).toJson());
        currentPartSize++;
        if (currentPartSize >= this->tableMetaInfo.getPartSize())
        {
            writePartData(&partDataMap);
            currentPartSize = 0;
            partDataMap.clear();
        }
    }
    if (currentPartSize > 0)
    {
        writePartData(&partDataMap);
    }


    this->tableMetaInfo.setDataLength((long long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart());
    this->tableMetaInfo.setIndexStart((long long)this->sSTableWriteStream.tellp());

    nlohmann::json sparseIndexJson(sparseIndex);
    auto sparseIndexJsonString = sparseIndexJson.dump();
    this->sSTableWriteStream.write(&sparseIndexJsonString[0], sparseIndexJsonString.size());
    this->sSTableWriteStream.flush();
    
    this->tableMetaInfo.setIndexLength((long long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getIndexStart());
    this->tableMetaInfo.writeToFile(this->sSTableWriteStream);
    this->sSTableWriteStream.flush();
}

void SSTable::writePartData(std::list<nlohmann::json> *partDataMap)
{
    nlohmann::json partDataMapJson(*partDataMap);
    auto partDataMapJsonString = partDataMapJson.dump();

    std::array<long, 2> c_array {{this->sSTableWriteStream.tellp(), (long)partDataMapJsonString.size()}};

    this->sparseIndex.insert(std::make_pair((*(*partDataMap).begin())["key"].get<std::string>(), c_array));

    this->sSTableWriteStream.write(&partDataMapJsonString[0], partDataMapJsonString.size());

    this->sSTableWriteStream.flush();
}

std::string SSTable::query(std::string key)
{
    std::cout << "Starting to query key: " << key.c_str() << std::endl;
    std::array<long, 2> *firstKeyPosition;
    std::array<long, 2> *lastKeyPosition;

    for (auto itr = sparseIndex.begin(); itr != sparseIndex.end(); itr++)
    {
        if (itr->first.compare(key) <= 0)
        {
            firstKeyPosition = &itr->second;
        } else {
            lastKeyPosition = &itr->second;
            break;
        }
    }

    if (firstKeyPosition)
    {
        std::map<std::string, std::shared_ptr<Command>> dataMap;
        std::string s = bufferedReader(firstKeyPosition->at(0), firstKeyPosition->at(1));
        nlohmann::json j = nlohmann::json::parse(s);

        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
            std::cout <<" : " << it.value() << "\n";
            auto command = convertToCommand(it.value());
            dataMap.insert(std::make_pair(command->getKey(), command));
        }
        auto dataCount = dataMap.count(key);
        if (dataCount > 0)
        {
            return dataMap[key]->getValue();
        }
    }

    if (lastKeyPosition)
    {
        std::map<std::string, std::shared_ptr<Command>> dataMap;
        std::string s = bufferedReader(firstKeyPosition->at(0), firstKeyPosition->at(1));
        nlohmann::json j = nlohmann::json::parse(s);

        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
            std::cout <<" : " << it.value() << "\n";
            auto command = convertToCommand(it.value());
            dataMap.insert(std::make_pair(command->getKey(), command));
        }
        auto dataCount = dataMap.count(key);
        if (dataCount > 0)
        {
            return dataMap[key]->getValue();
        }
    }

    return "";
}

std::string SSTable::bufferedReader(long seekOffset, long byteSize)
{
    std::string data(byteSize, '\0');
    this->sSTableReadStream.seekg(seekOffset, std::ios::beg);
    this->sSTableReadStream.read(&data[0], byteSize);
    return data;
}
