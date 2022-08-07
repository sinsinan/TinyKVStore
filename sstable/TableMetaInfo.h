//
// Created by ChistaData on 06/08/22.
//

#ifndef TINYKVSTORE_TABLEMETAINFO_H
#define TINYKVSTORE_TABLEMETAINFO_H

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
    void readFromFile(std::ifstream &file);
    void writeToFile(std::ofstream &file);
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



#endif //TINYKVSTORE_TABLEMETAINFO_H
