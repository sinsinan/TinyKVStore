//
// Created by ChistaData on 06/08/22.
//

#include<iostream>

#include "SetCommand.h"
#include <SSTable.h>

int main() {
    std::map<std::string, std::shared_ptr<Command> > data;

    data.insert(std::make_pair(std::string("sinan1"), std::make_shared<SetCommand>(SetCommand("sinan1", "ahamed1"))));
    data.insert(std::make_pair(std::string("sinan5"), std::make_shared<SetCommand>(SetCommand("sinan5", "ahamed5"))));
    data.insert(std::make_pair(std::string("sinan3"), std::make_shared<SetCommand>(SetCommand("sinan3", "ahamed3"))));
    data.insert(std::make_pair(std::string("sinan4"), std::make_shared<SetCommand>(SetCommand("sinan4", "ahamed4"))));
    data.insert(std::make_pair(std::string("sinan2"), std::make_shared<SetCommand>(SetCommand("sinan2", "ahamed2"))));

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