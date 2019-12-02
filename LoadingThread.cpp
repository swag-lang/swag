#include "pch.h"
#include "LoadingThread.h"
#include "SourceFile.h"
#include "Assert.h"
#include "ThreadManager.h"
#include "Os.h"
#include "Job.h"

LoadingThread::LoadingThread()
{
    thread = new std::thread(&LoadingThread::loop, this);
    OS::setThreadName(thread, "LoadingThread");
}

LoadingThread::~LoadingThread()
{
    //delete m_thread;
}

void LoadingThread::releaseRequest(LoadingThreadRequest* request)
{
    lock_guard<mutex> lk(mutexNew);
    freeRequests.push_back(request);
}

LoadingThreadRequest* LoadingThread::newRequest()
{
    lock_guard<mutex> lk(mutexNew);
    if (!freeRequests.empty())
    {
        auto req = freeRequests.back();
        freeRequests.pop_back();
        req->buffer     = nullptr;
        req->loadedSize = 0;
        req->done       = false;
        return req;
    }

    auto req = new LoadingThreadRequest;
    return req;
}

void LoadingThread::addRequest(LoadingThreadRequest* request)
{
    unique_lock lk(mutexAdd);
    queueRequests.push_back(request);
    condVar.notify_one();
}

LoadingThreadRequest* LoadingThread::getRequest()
{
    unique_lock lk(mutexAdd);
    if (queueRequests.empty())
        return nullptr;
    auto req = queueRequests.back();
    queueRequests.pop_back();
    return req;
}

void LoadingThread::waitRequest()
{
    unique_lock lk(mutexAdd);
    if (queueRequests.size())
        return;
    condVar.wait(lk);
}

void LoadingThread::loop()
{
    while (!requestEnd)
    {
        auto req = getRequest();
        if (req == nullptr)
        {
            if (requestEnd)
                break;
            waitRequest();
            continue;
        }

        req->loadedSize = 0;
        if (req->file->open())
        {
            req->file->seekTo(req->seek);
            req->loadedSize = req->file->readTo(req->buffer);
        }

        req->done = true;
        req->file->notifyLoad();
    }
}
