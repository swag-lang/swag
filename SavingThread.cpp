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
        req->ioError    = false;
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

void SavingThread::getRequests(vector<SaveThreadRequest*>& requests)
{
    lock_guard<mutex> lk(mutexAdd);
    requests.clear();
    if (queueRequests.empty())
        return;

    auto req = queueRequests.front();
    queueRequests.pop_front();
    requests.push_back(req);

    while (!queueRequests.empty())
    {
        auto frontReq = queueRequests.front();
		if (frontReq->file != req->file)
			break;
        queueRequests.pop_front();
        requests.push_back(frontReq);
    }
}

void SavingThread::waitRequest()
{
    unique_lock<mutex> lk(mutexAdd);
    if (!queueRequests.empty())
        return;
    condVar.wait(lk);
}

void SavingThread::loop()
{
    vector<SaveThreadRequest*> requests;
    while (!requestEnd)
    {
        getRequests(requests);
        if (requests.empty())
        {
            if (requestEnd)
                break;
            waitRequest();
            continue;
        }

        FILE* file     = nullptr;
        auto  frontReq = requests.front();
        for (int tryOpen = 0; tryOpen < 10; tryOpen++)
        {
            // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
            // Without that, fopen can fail due to compiling processes
            if (frontReq->firstSave)
                fopen_s(&file, frontReq->file->fileName.c_str(), "wtN");
            else
                fopen_s(&file, frontReq->file->fileName.c_str(), "a+tN");
            if (file)
                break;
            Sleep(10);
        }

        if (!file)
        {
            g_Log.error(format("cannot open file '%s' for writing (error %d)", frontReq->file->fileName.c_str(), errno));
            for (auto req : requests)
            {
                req->ioError = true;
                req->file->notifySave(req);
            }

            return;
        }

        for (auto req : requests)
        {
            fwrite(req->buffer, 1, req->bufferSize, file);
            req->file->notifySave(req);
        }

        fclose(file);
    }
}
