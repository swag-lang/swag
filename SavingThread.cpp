#include "pch.h"
#include "SavingThread.h"
#include "SourceFile.h"

SavingThread::SavingThread()
{
    thread = new std::thread(&SavingThread::loop, this);
}

SavingThread::~SavingThread()
{
    //delete m_thread;
}

void SavingThread::releaseRequest(SaveThreadRequest* request)
{
    lock_guard<mutex> lk(mutexNew);
    freeRequests.push_back(request);
}

SaveThreadRequest* SavingThread::newRequest()
{
    lock_guard<mutex> lk(mutexNew);
    if (!freeRequests.empty())
    {
        auto req = freeRequests.back();
        freeRequests.pop_back();
        req->buffer     = nullptr;
        req->bufferSize = 0;
        return req;
    }

    auto req = new SaveThreadRequest;
    return req;
}

void SavingThread::addRequest(SaveThreadRequest* request)
{
    lock_guard<mutex> lk(mutexAdd);
    queueRequests.push_back(request);
    condVar.notify_one();
}

SaveThreadRequest* SavingThread::getRequest()
{
    lock_guard<mutex> lk(mutexAdd);
    if (queueRequests.empty())
        return nullptr;
    auto req = queueRequests.back();
    queueRequests.pop_back();
    return req;
}

void SavingThread::waitRequest()
{
    unique_lock<mutex> lk(mutexAdd);
    condVar.wait(lk);
}

void SavingThread::loop()
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

        FILE* file = nullptr;
        fopen_s(&file, req->fileName, "w+t");
        if (file)
        {
            fwrite(req->buffer, 1, req->bufferSize, file);
            fclose(file);
        }
    }
}
