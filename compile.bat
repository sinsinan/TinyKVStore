g++ -I. -I.\utils\ -c .\utils\FileUtils.cpp

g++ -I. -I.\utils\ -c .\TableMetaInfo.cpp

g++ -I. -I.\utils\ -c .\SSTable.cpp

g++ -I. -I.\utils\ -c .\Main.cpp

g++ FileUtils.o TableMetaInfo.o SSTable.o Main.o -o output

.\output.exe