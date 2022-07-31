#include<SSTable.h>
#include<Command.h>
#include<list>
#include <memory>

int main() {
    std::map<std::string, std::shared_ptr<Command>> data;
    
    data.insert(std::make_pair(std::string("sinan1") , std::make_shared<SetCommand>(SetCommand("sinan1", "ahamed"))));
    data.insert(std::make_pair(std::string("sinan5"), std::make_shared<SetCommand>(SetCommand("sinan5", "ahamed"))));
    data.insert(std::make_pair(std::string("sinan3"), std::make_shared<SetCommand>(SetCommand("sinan3", "ahamed"))));
    data.insert(std::make_pair(std::string("sinan4"), std::make_shared<SetCommand>(SetCommand("sinan4", "ahamed"))));
    data.insert(std::make_pair(std::string("sinan2"), std::make_shared<SetCommand>(SetCommand("sinan2", "ahamed"))));
    // data.insert(dataPair);

    std::string tableFilePath = "C:\\Users\\ahamm\\sandbox\\tiny-kv-store\\datastore\\test.table";
    long partSize = 4;
    SSTable sstable(tableFilePath, partSize, data);
    sstable.~SSTable();

    SSTable sstableFromFile(tableFilePath);
    sstableFromFile.~SSTable();
    return 0;
}