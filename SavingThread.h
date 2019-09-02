#pragma once
struct OutputFile;

struct SaveThreadRequest
{
    OutputFile* file;
    char*       buffer;
    long        bufferSize;
    bool        ioError;
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
