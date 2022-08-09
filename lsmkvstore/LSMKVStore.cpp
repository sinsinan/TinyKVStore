//
// Created by ChistaData on 07/08/22.
//

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <chrono>
#include <cstdio>

#include <CommandUtil.h>
#include <LSMKVStore.h>

std::string addToDirectoryPath(std::string directoryPath, std::string pathToAdd) {
//    std::cout << "addToDirectoryPath: " << directoryPath + "/" + pathToAdd << std::endl;
    return directoryPath + "/" + pathToAdd;
}

inline bool endsWith(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string stringReaderFromFile(std::shared_ptr<std::fstream> file, long seekOffset, long byteSize)
{
    std::string data(byteSize, '\0');
    (*file).seekg(seekOffset, std::ios::beg);
    (*file).read(&data[0], byteSize);
    return data;
}


LSMKVStore::LSMKVStore(const std::string dataDirectory,const std::string tableName): wal(nullptr)  {
    this->tableName = tableName;
    this->dataDirectory = dataDirectory;
    restoreLSMKVStore();
    if (!this->wal) {
//        std::cout << "LSMKVStore::LSMKVStore: !this->wal" << std::endl;
        auto walPath = addToDirectoryPath(addToDirectoryPath(this->dataDirectory, this->tableName), WAL);
//        std::fstream walTemp = std::fstream(walPath, std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
        wal = std::make_shared<std::fstream>(std::fstream(walPath, std::ios::in | std::ios::out | std::ios::app | std::ios::binary));
    }
    this->currentCommandCount = this->dataIndex.size();
//    std::cout << "this->currentCommandCount: "<< this->currentCommandCount << std::endl;
}

void LSMKVStore::restoreLSMKVStore() {
    bool walSet = false;

    DIR *dir;
    struct dirent *ent;

    std::vector<std::string> tableFiles;

    std::string tableDirPath = addToDirectoryPath(this->dataDirectory, this->tableName);

    if ((dir = opendir (tableDirPath.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            tableFiles.push_back(ent->d_name);
        }
        closedir (dir);
    } else if (mkdir(tableDirPath.c_str(), 0777) == -1) {
        std::cout << "Could not use given data dir" << std::endl;
        throw "Could not use given data dir";
    }

    std::map<long, std::shared_ptr<SSTable> > ssTableMap;
    for (auto itr = tableFiles.begin(); itr != tableFiles.end() ; itr++) {
        if (endsWith(*itr, TABLE)){
            auto dotIndex = (*itr).find('.');
            try {
                ssTableMap[stol((*itr).substr(0, dotIndex))] = std::make_shared<SSTable>(SSTable(addToDirectoryPath(tableDirPath, *itr)));
            }
            catch (nlohmann::json_v3_11_1::detail::parse_error e) {
                std::cout << "Error while parsing data file: " << (*itr) << ", Exception: " << e.what() << std::endl;
            }
        } else if((*itr) == WAL) {
            wal = std::make_shared<std::fstream>(std::fstream(addToDirectoryPath(tableDirPath, (*itr)), std::ios::in | std::ios::out | std::ios::app | std::ios::binary));
            walSet = true;
            restoreFromWAL(wal);
        } else if ((*itr) == WAL_TEMP) {
             restoreFromWAL(std::make_shared<std::fstream>(std::fstream(addToDirectoryPath(tableDirPath, (*itr)), std::ios::in | std::ios::out | std::ios::app | std::ios::binary)));
        }
    }

//    long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

//    ssTableMap[timestamp] = std::make_shared<SSTable>(SSTable(addToDirectoryPath(tableDirPath, std::to_string(timestamp)+TABLE), commandPartSize));

    for (auto itr = ssTableMap.begin(); itr != ssTableMap.end() ; itr++) {
        ssTables.push_back(itr->second);
    }

    if (!walSet) {
//        std::fstream walTemp = std::fstream(addToDirectoryPath(tableDirPath, WAL), std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
        wal = std::make_shared<std::fstream>(std::fstream(addToDirectoryPath(tableDirPath, WAL), std::ios::in | std::ios::out | std::ios::app | std::ios::binary));
    }

//    std::cout << "Done" <<std::endl;

}

void LSMKVStore::restoreFromWAL(std::shared_ptr<std::fstream> rWalFile) {
    rWalFile->seekg(0, std::ios::end);
    long fileLength = (*rWalFile).tellg();

    long start = 0;
    rWalFile->seekg(0, std::ios::beg);
    fileLength = fileLength - (*rWalFile).tellg();

//    std::cout << "restoreFromWAL: " << "fileLength: " << fileLength << std::endl;
    while (start < fileLength) {
        long commandLength;
        rWalFile->read((char *)&commandLength, sizeof(long));
        start += sizeof(long);
        std::string commandString = stringReaderFromFile(rWalFile, start, commandLength);
        start += commandLength;

        nlohmann::json commandJSON = nlohmann::json::parse(commandString);

        auto command = convertToCommand(commandJSON);

        this->dataIndex[command->getKey()] = command;
    }
    rWalFile->seekg(0, std::ios::end);

}

void LSMKVStore::set(std::string key, std::string value) {
    addCommandToIndex(std::make_shared<SetCommand>(SetCommand(key, value)));
}

void LSMKVStore::addCommandToIndex(std::shared_ptr<Command> command) {
    this->wal->seekp(0, std::ios::end);
    this->dataIndex[command->getKey()] = command;
    std::string commandString = nlohmann::json(command->toJson()).dump();
    long commandStringSize = commandString.size();
    this->wal->write((char*)&commandStringSize, sizeof(long));
    this->wal->flush();
    this->wal->write(&commandString[0], commandStringSize);
    this->wal->flush();
    this->currentCommandCount += 1;
    if (this->currentCommandCount >= this->commandThreshold) {
        switchIndex();
        writeToSSTable();
        this->currentCommandCount = 0;
    }
}

void LSMKVStore::rm(std::string key) {
    addCommandToIndex(std::make_shared<RmCommand>(RmCommand(key)));
}

void LSMKVStore::switchIndex() {
    this->immutableDataIndex = this->dataIndex;
    this->wal->close();
    auto walTempPath = addToDirectoryPath(addToDirectoryPath(this->dataDirectory, this->tableName), WAL_TEMP);
    remove(walTempPath.c_str()); //TODO add checks

    auto walPath = addToDirectoryPath(addToDirectoryPath(this->dataDirectory, this->tableName), WAL);
    rename(walPath.c_str(), walTempPath.c_str());
//    auto walTemp = std::fstream(walPath, std::ios::in | std::ios::out | std::ios::app | std::ios::binary);
    wal = std::make_shared<std::fstream>(std::fstream(walPath, std::ios::in | std::ios::out | std::ios::app | std::ios::binary));
}

void LSMKVStore::writeToSSTable() {
    long timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::string sSTablePath = addToDirectoryPath(addToDirectoryPath(this->dataDirectory, this->tableName), std::to_string( timestamp) + TABLE);
    this->ssTables.push_back(std::make_shared<SSTable>(SSTable(sSTablePath, this->commandPartSize, this->dataIndex)));
    this->dataIndex.clear();
}

std::string LSMKVStore::get(std::string key) {
    if (this->dataIndex.count(key) > 0){
        return this->dataIndex[key]->getValue();
    }

    if (this->immutableDataIndex.count(key) > 0){
        return this->immutableDataIndex[key]->getValue();
    }

    for (auto itr = ssTables.begin(); itr < ssTables.end(); ++itr) {
        auto command = itr->get()->query(key);
        if(command){
            return command->getValue();
        }
    }

    return "";
}

void LSMKVStore::close() {
    if(*this->wal) {
        this->wal->close();
    }
//    std::cout<<"Closing LSMKVStore" <<std::endl;
}

std::string LSMKVStore::getTableName() {
    return this->tableName;
}
