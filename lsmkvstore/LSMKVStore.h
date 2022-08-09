//
// Created by ChistaData on 07/08/22.
//

#ifndef TINYKVSTORE_LSMKVSTORE_H
#define TINYKVSTORE_LSMKVSTORE_H

#include<shared_mutex>
#include<string>
#include<fstream>

#include<Command.h>
#include<SSTable.h>


class LSMKVStore {
private:
    const std::string WAL = "wal";
    const std::string WAL_TEMP = "wal-temp";
    const std::string TABLE = ".table";

    const long commandThreshold = 25;
    const long commandPartSize = 5;

//    typedef std::shared_timed_mutex Lock;
//    std::unique_lock< Lock >  IndexWriteLock;
//    std::shared_lock< Lock >  IndexReadLock;

    long currentCommandCount;

    std::string tableName;
    std::string dataDirectory;

    std::shared_ptr<std::fstream> wal;

    std::map<std::string, std::shared_ptr<Command> > dataIndex;
    std::map<std::string, std::shared_ptr<Command> > immutableDataIndex;

    std::vector<std::shared_ptr<SSTable>> ssTables;

    void writeToSSTable();
    void restoreLSMKVStore();
    void restoreFromWAL(std::shared_ptr<std::fstream> rWalFile);
    void addCommandToIndex(std::shared_ptr<Command>);
    void switchIndex();
public:
    LSMKVStore(std::string tableName, std::string dataDirectory);

    std::string get(std::string key);
    std::string getTableName();
    void set(std::string key, std::string value);
    void rm(std::string key);
    void close();
};


#endif //TINYKVSTORE_LSMKVSTORE_H
