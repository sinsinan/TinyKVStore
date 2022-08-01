#include <fstream>
#include <TableMetaInfo.h>

void TableMetaInfo::readFromFile(std::ifstream &file)
{
    // char *buffer = new char[4];
    file.seekg(0, std::ios::end );

    // printf("TableMetaInfo::readFromFile file length: %ld\n", file.tellg());

    file.seekg((long long)file.tellg() - (4 * 6));

    file.read((char *)&this->partSize, 4);
    file.read((char *)&this->dataStart, 4);
    file.read((char *)&this->dataLength, 4);
    file.read((char *)&this->indexStart, 4);
    file.read((char *)&this->indexLength, 4);
    file.read((char *)&this->version, 4);

    printf("\nthis->partSize: %d\nthis->dataStart: %d\nthis->dataLength: %d\nthis->indexStart: %d\nthis->indexLength: %d\nthis->version: %d\n", this->partSize, this->dataStart, this->dataLength, this->indexStart, this->indexLength, this->version);
}
void TableMetaInfo::writeToFile(std::ofstream &file)
{
    file.write((char *)&this->partSize, 4);
    file.write((char *)&this->dataStart, 4);
    file.write((char *)&this->dataLength, 4);
    file.write((char *)&this->indexStart, 4);
    file.write((char *)&this->indexLength, 4);
    file.write((char *)&this->version, 4);

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