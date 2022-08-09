//
// Created by ChistaData on 06/08/22.
//

#include<iostream>
#include <string>
#include <sstream>

#include "SetCommand.h"
#include <SSTable.h>

#include <LSMKVStore.h>

void printHelp(){
    std::cout<<"Tiny Key Value Store -- Help" << std::endl;
    std::cout<<"kvstore <data-dir> <table-name>"<< std::endl;
}

void printCommandsAvailable() {
    std::cout<<"commands available"<< std::endl;
    std::cout<<"set <key> <value>"<< std::endl;
    std::cout<<"get <key>"<< std::endl;
    std::cout<<"rm <key>"<< std::endl;
    std::cout<<"exit"<< std::endl;
}

std::vector<std::string> simpleTokenizer(std::string s)
{
    std::vector<std::string> commandTokens;
    std::stringstream ss(s);
    std::string word;
    while (ss >> word) {
        commandTokens.push_back(word);
    }
    return commandTokens;
}


int main(int argc, char **argv) {
    if(argc != 3) {
        printHelp();
        return -1;
    }

    std::shared_ptr<LSMKVStore> currentStore = nullptr;

    currentStore = std::make_shared<LSMKVStore>(LSMKVStore(std::string(argv[1]), std::string(argv[2])));

    bool exit = false;
    std::string commandString;
    std::vector<std::string> commandTokens;
    while (!exit) {
        if(currentStore) {
            std::cout << currentStore->getTableName();
        }
        std::cout << " > ";
        getline(std::cin, commandString);


        std::vector<std::string> commandTokens = simpleTokenizer(commandString);

        if (commandTokens.size() > 0) {
            if(commandTokens[0] == "exit") {
                exit = true;
            } else if(commandTokens[0] == "get") {
                std::cout << /*"key: " << commandTokens[1] <<" , value: " <<*/  currentStore->get(commandTokens[1]) << std::endl;
            } else if(commandTokens[0] == "rm") {
                currentStore->rm(commandTokens[1]);
            } else if(commandTokens[0] == "set") {
                currentStore->set(commandTokens[1], commandTokens[2]);
            } else{
                printCommandsAvailable();
            }
        } else {
            printCommandsAvailable();

        }
    }

    return 0;
}