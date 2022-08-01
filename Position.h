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

#endif