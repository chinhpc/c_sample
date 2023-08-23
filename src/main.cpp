#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>      // std::ifstream
#include <string>
#include <ziparchive/zip_writer.h>

#if 0
#define PRINTD(...) printf(__VA_ARGS__)
#else
#define PRINTD(...)
#endif

namespace fs = std::filesystem;

//------------------------------------------------------------------------------------------------------------------------
// @Function        : makeZipFile
// @Description     : Make a zip file contain the list of file tree
//
// @Parameters      : filename: Zip file name, path: Directory to zip
// @Return Value    : N/A

bool makeZipFile(const char *filename, const fs::path& path) {
    PRINTD("Start makeZipFile(%s)\n", filename);

    FILE* file = NULL;
    fs::path prev_path = fs::current_path();

    if ( (file = fopen(filename, "wb")) == NULL) {
        PRINTD("Error! Cannot open ZipFile\n");
        return false;
    }

    ZipWriter writer(file);

    fs::current_path(path);  //set cur path first

    //get file list
    for(const auto& p: fs::recursive_directory_iterator(".")) {
        if (!fs::is_directory(p)) {
            std::ifstream is (p.path(), std::ifstream::binary);
            if (is) {
                // get length of file:
                is.seekg(0, is.end);
                int length = is.tellg();
                is.seekg(0, is.beg);

                char * buffer = new char [length];
                // read data as a block:
                is.read(buffer,length);
                if (is)
                    PRINTD("all characters read successfully.\n");
                else
                    PRINTD("error: only %l could be read\n", is.gcount());

                writer.StartEntry(p.path().string().c_str(), ZipWriter::kCompress | ZipWriter::kAlign32);
                writer.WriteBytes(buffer, length);
                writer.FinishEntry();
                is.close();
                delete[] buffer;
            }
        }
    }

    fs::current_path(prev_path);  //set cur path back

    writer.Finish();

    if (fclose(file) != 0)
    {
        PRINTD("Error! Cannot close ZipFile: %s.\n", filename);
        return false;
    }

    PRINTD("End makeZipFile()\n");

    return true;
}

int main(int argc, char** argv)
{
    try{
        if( argc != 3)
            throw std::invalid_argument("Invalid argument.");

        for(const auto& p: std::vector<fs::path> {argv[1], argv[2]}){
            if (fs::is_directory(p)){
                std::string zipFullFileName;
                zipFullFileName.append(p.string());
                zipFullFileName.append(".zip");
                if(!makeZipFile(zipFullFileName.c_str(), p))
                    printf("makeZipFile return failed.\n");
           }
           else{
                PRINTD("%s is not a dir\n", p.string().c_str());
           }
        }
    }
    catch(const std::exception& ex){
        printf("%s\n",ex.what());
        return 0;
    }

    return 0;
}
