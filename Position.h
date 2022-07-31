#ifndef KEYVALUE_INDEX_POSITION_H
#define KEYVALUE_INDEX_POSITION_H

class IndexPosition
{
private:
    long offset;
    long length;

public:
    IndexPosition(long offset, long length);
    long getOffset();
    long getLength();
    ~IndexPosition();
};

IndexPosition::IndexPosition(long offset, long length)
{
    this->offset = offset;
    this->length = length;
}

long IndexPosition::getOffset()
{
    return this->offset;
}
long IndexPosition::getLength()
{
    return this->length;
}

IndexPosition::~IndexPosition()
{
}

#endif