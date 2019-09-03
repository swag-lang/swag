#pragma once
struct OutputFile;

struct SaveThreadRequest
{
    OutputFile* file       = nullptr;
    char*       buffer     = nullptr;
    long        bufferSize = 0;
    bool        ioError    = false;
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

    thread*                   thread     = nullptr;
    bool                      requestEnd = false;
    deque<SaveThreadRequest*> queueRequests;
    deque<SaveThreadRequest*> freeRequests;
    mutex                     mutexAdd;
    mutex                     mutexNew;
    condition_variable        condVar;
};
