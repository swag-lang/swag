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

    thread*                       m_thread     = nullptr;
    bool                          m_requestEnd = false;
    vector<LoadingThreadRequest*> m_queue;
    vector<LoadingThreadRequest*> m_freeRequests;
    mutex                         m_mutexAdd;
    mutex                         m_mutexNew;
    condition_variable            m_Cv;
};
