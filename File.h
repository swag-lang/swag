#pragma once
struct SourceFile;
struct OutputFile;

struct LoadRequest
{
    SourceFile* file;
    char*       buffer;
    long        seek;
    bool        done       = false;
    long        loadedSize = 0;
};

struct SaveRequest
{
    OutputFile* file       = nullptr;
    char*       buffer     = nullptr;
    long        bufferSize = 0;
    bool        ioError    = false;
    //bool        lastOne    = false;
    //bool        flush      = false;
};

struct File
{
    bool openRead();
    bool openWrite();
    void close();

    static void openFile(FILE** fileHandle, const char* path, const char* mode);
    static void closeFile(FILE** fileHandle);

    static void load(LoadRequest* request);
    static void save(SaveRequest* request);

    string path;
    FILE*  fileHandle = nullptr;
    bool   openedOnce = false;
};
