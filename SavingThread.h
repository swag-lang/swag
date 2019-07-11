#pragma once
struct SourceFile;

struct SaveThreadRequest
{
    const char* fileName;
    char*       buffer;
    long        bufferSize;
};

struct SavingThread
{
    SavingThread();
    ~SavingThread();
    void               addRequest(SaveThreadRequest* request);
    void               releaseRequest(SaveThreadRequest* request);
    SaveThreadRequest* newRequest();

    void               loop();
    SaveThreadRequest* getRequest();
    void               waitRequest();

    thread*                    thread     = nullptr;
    bool                       requestEnd = false;
    vector<SaveThreadRequest*> queueRequests;
    vector<SaveThreadRequest*> freeRequests;
    mutex                      mutexAdd;
    mutex                      mutexNew;
    condition_variable         condVar;
};
