#include "pch.h"
#include "IoThread.h"
#include "SourceFile.h"
#include "OutputFile.h"
#include "Assert.h"
#include "ThreadManager.h"
#include "Os.h"
#include "Job.h"

IoThread::IoThread()
{
    thread = new std::thread(&IoThread::loop, this);
    OS::setThreadName(thread, "IOThread");
}

void IoThread::releaseLoadingRequest(LoadingThreadRequest* request)
{
    unique_lock lk(mutexNew);
    freeLoadingRequests.push_back(request);
}

LoadingThreadRequest* IoThread::newLoadingRequest()
{
    unique_lock lk(mutexNew);
    if (!freeLoadingRequests.empty())
    {
        auto req = freeLoadingRequests.back();
        freeLoadingRequests.pop_back();
        req->buffer     = nullptr;
        req->loadedSize = 0;
        req->done       = false;
        return req;
    }

    auto req = new LoadingThreadRequest;
    return req;
}

void IoThread::addLoadingRequest(LoadingThreadRequest* request)
{
    unique_lock lk(mutexAdd);
    queueLoadingRequests.push_back(request);
    condVar.notify_one();
}

LoadingThreadRequest* IoThread::getLoadingRequest()
{
    unique_lock lk(mutexAdd);
    if (queueLoadingRequests.empty())
        return nullptr;
    auto req = queueLoadingRequests.back();
    queueLoadingRequests.pop_back();
    return req;
}

void IoThread::waitRequest()
{
    unique_lock lk(mutexAdd);
    if (queueLoadingRequests.size())
        return;
    if (queueSavingRequests.size())
        return;
    condVar.wait(lk);
}

void IoThread::releaseSavingRequest(SavingThreadRequest* request)
{
    unique_lock lk(mutexNew);
    freeSavingRequests.push_back(request);
}

SavingThreadRequest* IoThread::newSavingRequest()
{
    unique_lock lk(mutexNew);
    if (!freeSavingRequests.empty())
    {
        auto req = freeSavingRequests.back();
        freeSavingRequests.pop_back();
        req->file       = nullptr;
        req->buffer     = nullptr;
        req->bufferSize = 0;
        req->ioError    = false;
        req->lastOne    = false;
        return req;
    }

    auto req = new SavingThreadRequest;
    return req;
}

void IoThread::addSavingRequest(SavingThreadRequest* request)
{
    unique_lock lk(mutexAdd);
    queueSavingRequests.push_back(request);
    condVar.notify_one();
}

SavingThreadRequest* IoThread::getSavingRequest()
{
    unique_lock lk(mutexAdd);
    if (queueSavingRequests.empty())
        return nullptr;

    auto req = queueSavingRequests.front();
    queueSavingRequests.pop_front();
    return req;
}

void IoThread::save(SavingThreadRequest* request)
{
    FILE* file = request->file->openFile;
    if (!file)
    {
        for (int tryOpen = 0; tryOpen < 10; tryOpen++)
        {
            // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
            // Without that, fopen can fail due to compiling processes
            if (request->firstSave)
                fopen_s(&file, request->file->fileName.c_str(), "wtN");
            else
                fopen_s(&file, request->file->fileName.c_str(), "a+tN");
			if (file)
			{
				setvbuf(file, nullptr, _IONBF, 0);
				break;
			}

            Sleep(10);
        }

        request->file->openFile = file;
    }

    if (!file)
    {
        g_Log.error(format("cannot open file '%s' for writing (error %d)", request->file->fileName.c_str(), errno));
        return;
    }

    fwrite(request->buffer, 1, request->bufferSize, file);

    if (request->lastOne)
    {
        fclose(file);
        request->file->openFile = nullptr;
        request->file->notifySave();
    }

    releaseSavingRequest(request);
}

void IoThread::load(LoadingThreadRequest* request)
{
    request->loadedSize = 0;
    if (request->file->open())
    {
        request->file->seekTo(request->seek);
        request->loadedSize = request->file->readTo(request->buffer);
    }

    request->done = true;
    request->file->notifyLoad();
}

void IoThread::loop()
{
    while (true)
    {
        auto loadingReq = getLoadingRequest();
        if (loadingReq)
        {
            load(loadingReq);
            continue;
        }

        auto savingReq = getSavingRequest();
        if (savingReq)
        {
            save(savingReq);
            continue;
        }

        waitRequest();
    }
}
