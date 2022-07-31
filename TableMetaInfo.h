#ifndef TABLE_META_INFO_H
#define TABLE_META_INFO_H

#include <fstream>

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

#endif