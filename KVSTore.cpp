#include <fstream>
#include <map>
#include "KVStore.h"
#include <string>
#include <list>
#include <iostream>
#include <sstream>

#include "json.h"

void TableMetaInfo::readFromFile(std::ifstream &file)
{
    auto longSize = sizeof(long);
    // char *buffer = new char[4];
    file.seekg(0, std::ios::end );

    // printf("TableMetaInfo::readFromFile file length: %ld\n", file.tellg());

    file.seekg((long long)file.tellg() - (longSize * 6));

    file.read((char *)&this->partSize, longSize);
    file.read((char *)&this->dataStart, longSize);
    file.read((char *)&this->dataLength, longSize);
    file.read((char *)&this->indexStart, longSize);
    file.read((char *)&this->indexLength, longSize);
    file.read((char *)&this->version, longSize);

    printf("\nthis->partSize: %d\nthis->dataStart: %d\nthis->dataLength: %d\nthis->indexStart: %d\nthis->indexLength: %d\nthis->version: %d\n", this->partSize, this->dataStart, this->dataLength, this->indexStart, this->indexLength, this->version);
}
void TableMetaInfo::writeToFile(std::ofstream &file)
{
    auto longSize = sizeof(long);
    file.write((char *)&this->partSize, longSize);
    file.write((char *)&this->dataStart, longSize);
    file.write((char *)&this->dataLength, longSize);
    file.write((char *)&this->indexStart, longSize);
    file.write((char *)&this->indexLength, longSize);
    file.write((char *)&this->version, longSize);
    printf("\nthis->partSize: %d\nthis->dataStart: %d\nthis->dataLength: %d\nthis->indexStart: %d\nthis->indexLength: %d\nthis->version: %d\n", this->partSize, this->dataStart, this->dataLength, this->indexStart, this->indexLength, this->version);
}

long TableMetaInfo::getVersion()
{
    return this->version;
}

long TableMetaInfo::getDataStart()
{
    return this->dataStart;
}

long TableMetaInfo::getDataLength()
{
    return this->dataLength;
}

long TableMetaInfo::getIndexStart()
{
    return this->indexStart;
}

long TableMetaInfo::getIndexLength()
{
    return this->indexLength;
}

long TableMetaInfo::getPartSize()
{
    return this->partSize;
}

void TableMetaInfo::setVersion(long version)
{
    this->version = version;
}

void TableMetaInfo::setDataStart(long dataStart)
{
    this->dataStart = dataStart;
}

void TableMetaInfo::setDataLength(long dataLength)
{
    this->dataLength = dataLength;
}

void TableMetaInfo::setIndexStart(long indexStart)
{
    this->indexStart = indexStart;
}

void TableMetaInfo::setIndexLength(long indexLength)
{
    this->indexLength = indexLength;
}

void TableMetaInfo::setPartSize(long partSize)
{
    this->partSize = partSize;
}






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

// SSTable::~SSTable()
// {
//     printf("SSTable destructor called\n");
//     this->sSTableReadStream.close();
//     this->sSTableWriteStream.close();
// }

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
        printf("\n!this->sSTableWriteStream failed\n");
        throw "this->sSTableWriteStream failed";
    }
    printf("\nopened sSTableWriteStream\n");
    this->sSTableReadStream.open(this->filePath, std::ios_base::binary | std::ios_base::app | std::ios_base::in);
    if (!this->sSTableReadStream)
    {
        printf("\nthis->sSTableReadStream.open failed\n");
        throw "this->sSTableReadStream.open failed";
    }
    printf("\nopened SSTableReadStream\n");
}

void SSTable::initFromData(std::map<std::string, std::shared_ptr<Command> > data)
{
    printf("Loading table from index\n");
    printf("total data -> %d\n", data.size());
    this->tableMetaInfo.setDataStart(this->sSTableWriteStream.tellp());
    long currentPartSize = 0;
    std::list<nlohmann::json> partDataMap;
    for (auto itr = data.begin(); itr != data.end(); ++itr)
    {
        partDataMap.push_back((*itr->second).toJson());
        currentPartSize++;
        if (currentPartSize >= this->tableMetaInfo.getPartSize())
        {
            nlohmann::json partDataMapJson(partDataMap);
            auto partDataMapJsonString = partDataMapJson.dump();

            std::array<long, 2> c_array {{this->sSTableWriteStream.tellp() /*- this->tableMetaInfo.getDataStart()*/, (long)partDataMapJsonString.size()}};

            this->sparseIndex.insert(std::make_pair((*partDataMap.begin())["key"].get<std::string>(), c_array));

            this->sSTableWriteStream.write(&partDataMapJsonString[0], partDataMapJsonString.size());

            this->sSTableWriteStream.flush();

            currentPartSize = 0;
            partDataMap.clear();
        }
    }
    if (currentPartSize > 0)
    {
        nlohmann::json partDataMapJson(partDataMap);
        auto partDataMapJsonString = partDataMapJson.dump();

        std::array<long, 2> c_array {{this->sSTableWriteStream.tellp() /*- this->tableMetaInfo.getDataStart()*/, (long)partDataMapJsonString.size()}};

        this->sparseIndex.insert(std::make_pair((*partDataMap.begin())["key"].get<std::string>(), c_array));

        this->sSTableWriteStream.write(&partDataMapJsonString[0], partDataMapJsonString.size());

        this->sSTableWriteStream.flush();

        currentPartSize = 0;
        partDataMap.clear();
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

void SSTable::writePartData(std::map<std::string, std::map<std::string, std::string> > partData)
{
    // this->sparseIndex.insert(std::make_pair(partData.begin()->first, std::make_pair((long)this->sSTableWriteStream.tellp() - this->tableMetaInfo.getDataStart(), sizeof(partData))));
    // this->sSTableWriteStream.write((char *)&partData, sizeof(partData));
}

std::string SSTable::query(std::string key)
{
    printf("Starting to query key:%s\n", key.c_str());
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

std::shared_ptr<Command>  SSTable::convertToCommand(nlohmann::json j){
    switch (j["commandType"].get<int>())
    {
    case SET:
        return std::make_shared<SetCommand>(SetCommand(j));
    default:
        return std::make_shared<RmCommand>(RmCommand(j));
    }
}

std::string SSTable::bufferedReader(long seekOffset, long byteSize)
{
    std::string data(byteSize, '\0');
    this->sSTableReadStream.seekg(seekOffset, std::ios::beg);
    this->sSTableReadStream.read(&data[0], byteSize);
    return data;
}


//IndexPosition::IndexPosition(long offset, long length)
//{
//    this->offset = offset;
//    this->length = length;
//}

//long IndexPosition::getOffset()
//{
//    return this->offset;
//}
//long IndexPosition::getLength()
//{
//    return this->length;
//}

//IndexPosition::~IndexPosition()
//{
//}





int main() {
    std::map<std::string, std::shared_ptr<Command> > data;

    data.insert(std::make_pair(std::string("sinan1"), std::make_shared<SetCommand>(SetCommand("sinan1", "ahamed1"))));
    data.insert(std::make_pair(std::string("sinan5"), std::make_shared<SetCommand>(SetCommand("sinan5", "ahamed5"))));
    data.insert(std::make_pair(std::string("sinan3"), std::make_shared<SetCommand>(SetCommand("sinan3", "ahamed3"))));
    data.insert(std::make_pair(std::string("sinan4"), std::make_shared<SetCommand>(SetCommand("sinan4", "ahamed4"))));
    data.insert(std::make_pair(std::string("sinan2"), std::make_shared<SetCommand>(SetCommand("sinan2", "ahamed2"))));
    // data.insert(dataPair);

//    std::list<SetCommand> ss;
//    ss.push_back(SetCommand("sinan1", "ahamed1"));
//    ss.push_back(SetCommand("sinan3", "ahamed3"));
//    ss.push_back(SetCommand("sinan2", "ahamed2"));
//    ss.push_back(SetCommand("sinan4", "ahamed4"));

//    std::list<nlohmann::json> sj;
//    for (auto i : ss) {
//        sj.push_back(i.toJson());
//    }

//    auto s2 = nlohmann::json(sj).dump();

//    auto pj = nlohmann::json::parse(s2);

//    for (auto i : pj) {
//        std::cout<<"pj: "<<i <<std::endl;
//    }




    std::string tableFilePath = "/Users/chistadata/Documents/TinyKVStore/test.table";
    long partSize = 2;

    SSTable sstable(tableFilePath, partSize, data);
    auto sr = sstable.query("sinan3");

    std::cout<<"sinan3: "<<sr<<std::endl;

    SSTable sstableFromFile(tableFilePath);
    sr = sstableFromFile.query("sinan5");
    std::cout<<"sinan5: "<<sr<<std::endl;
    return 0;
}
