#pragma once
struct SourceFile;

struct LoadingThreadRequest
{
    SourceFile* file;
    char*       buffer;
    long        seek;
    bool        done       = false;
    long        loadedSize = 0;
};

struct LoadingThread
{
    LoadingThread();
    ~LoadingThread();
    void                  addRequest(LoadingThreadRequest* request);
    void                  releaseRequest(LoadingThreadRequest* request);
    LoadingThreadRequest* newRequest();

    void                  loop();
    LoadingThreadRequest* getRequest();
    void                  waitRequest();

    thread*                       thread     = nullptr;
    bool                          requestEnd = false;
    vector<LoadingThreadRequest*> queueRequests;
    vector<LoadingThreadRequest*> freeRequests;
    mutex                         mutexAdd;
    mutex                         mutexNew;
    condition_variable            condVar;
};
