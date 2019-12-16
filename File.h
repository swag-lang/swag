#pragma once

struct File
{
    bool openRead();
    void close();

    fs::path path;
    FILE*    fileHandle = nullptr;
    bool     openedOnce = false;
};
