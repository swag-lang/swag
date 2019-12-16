#pragma once
struct File
{
    bool openRead();
    bool openWrite();
    void close();

    static void openFile(FILE** fileHandle, const char* path, const char* mode);
    static void closeFile(FILE** fileHandle);

    string path;
    FILE*  fileHandle = nullptr;
    bool   openedOnce = false;
};
