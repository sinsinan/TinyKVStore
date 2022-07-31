#include <fstream>
#include <FileUtils.h>

long getFileSize(std::ifstream &file){
    std::streampos fsize = 0;
    fsize = file.tellg();
    file.seekg( 0, std::ios::end );
    return file.tellg() - fsize;
}