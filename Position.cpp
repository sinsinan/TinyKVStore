#include<Position.h>

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