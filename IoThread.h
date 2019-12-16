#pragma once
struct SourceFile;
struct OutputFile;

struct LoadingThreadRequest
{
    SourceFile* file;
    char*       buffer;
    long        seek;
    bool        done       = false;
    long        loadedSize = 0;
};

struct SavingThreadRequest
{
    OutputFile* file       = nullptr;
    char*       buffer     = nullptr;
    long        bufferSize = 0;
    bool        ioError    = false;
    bool        lastOne    = false;
    bool        flush      = false;
};

struct IoThread
{
    IoThread();

    static void openFile(FILE** fileHandle, const char* path, const char* mode, bool raiseError = true);
    static void closeFile(FILE** fileHandle);

    void                  addLoadingRequest(LoadingThreadRequest* request);
    void                  releaseLoadingRequest(LoadingThreadRequest* request);
    LoadingThreadRequest* newLoadingRequest();

    void                 addSavingRequest(SavingThreadRequest* request);
    void                 releaseSavingRequest(SavingThreadRequest* request);
    SavingThreadRequest* newSavingRequest();

    LoadingThreadRequest* getLoadingRequest();
    SavingThreadRequest*  getSavingRequest();
    void                  waitRequest();

    void loop();
    void load(LoadingThreadRequest* request);
    void save(SavingThreadRequest* request);

    vector<LoadingThreadRequest*> queueLoadingRequests;
    vector<LoadingThreadRequest*> freeLoadingRequests;
    vector<SavingThreadRequest*>  queueSavingRequests;
    vector<SavingThreadRequest*>  freeSavingRequests;

    thread*            thread = nullptr;
    mutex              mutexAdd;
    mutex              mutexNew;
    condition_variable condVar;
};
