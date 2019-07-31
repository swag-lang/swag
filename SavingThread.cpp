#include "pch.h"
#include "SavingThread.h"
#include "OutputFile.h"
#include "Log.h"

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
        req->file       = nullptr;
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
    auto req = queueRequests.front();
    queueRequests.pop_front();
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
        for (int tryOpen = 0; tryOpen < 10; tryOpen++)
        {
            fopen_s(&file, req->file->fileName.c_str(), "a+t");
            if (file)
                break;
            Sleep(10);
        }

        if (file)
        {
            fwrite(req->buffer, 1, req->bufferSize, file);
            fclose(file);
        }
        else
        {
            g_Log.error(format("cannot open file '%s' for writing (error %d)", req->file->fileName.c_str(), errno));
        }

        req->file->notifySave(req);
    }
}
