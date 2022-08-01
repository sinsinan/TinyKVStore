g++ -I. -I.\utils\ -c .\Position.cpp
g++ -I. -I.\utils\ -c .\TableMetaInfo.cpp
g++ -I. -I.\utils\ -c .\SSTable.cpp
g++ -I. -I.\utils\ -c .\Main.cpp
g++ TableMetaInfo.o SSTable.o Main.o Position.o -o output
.\output.exe

clang++ -Wall -std=c++11 -I. TableMetaInfo.cpp SSTable.cpp Main.cpp Position.cpp -o output