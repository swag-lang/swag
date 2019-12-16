#include "pch.h"
#include "IoThread.h"
#include "SourceFile.h"
#include "OutputFile.h"
#include "Os.h"
#include "Stats.h"

IoThread::IoThread()
{
    thread = new std::thread(&IoThread::loop, this);
    OS::setThreadName(thread, "IOThread");
}

void IoThread::openFile(FILE** fileHandle, const char* path, const char* mode, bool raiseError)
{
    *fileHandle = nullptr;
    auto err    = fopen_s(fileHandle, path, mode);
    if (fileHandle == nullptr)
    {
        if (raiseError)
        {
            char buf[256];
            strerror_s(buf, err);
            g_Log.error(format("error opening file '%s': '%s'", path, buf));
        }

        return;
    }

    g_Stats.numOpenFiles++;
    g_Stats.maxOpenFiles = max(g_Stats.maxOpenFiles.load(), g_Stats.numOpenFiles.load());
}

void IoThread::closeFile(FILE** fileHandle)
{
    if (!*fileHandle)
        return;
    fclose(*fileHandle);
    *fileHandle = nullptr;
    g_Stats.numOpenFiles--;
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

    // Priority to an already open file, in order to reduce number of open files
    // at the same time
    LoadingThreadRequest* request = nullptr;
    for (auto it = queueLoadingRequests.begin(); it != queueLoadingRequests.end(); ++it)
    {
        if ((*it)->file->fileHandle)
        {
            request = *it;
            queueLoadingRequests.erase(it);
            break;
        }
    }

    if (request == nullptr)
    {
        request = queueLoadingRequests.back();
        queueLoadingRequests.pop_back();
    }

    return request;
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
        req->flush      = false;
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

    // Priority to an already open file, in order to reduce number of open files
    // at the same time
    SavingThreadRequest* request = nullptr;
    for (auto it = queueSavingRequests.begin(); it != queueSavingRequests.end(); ++it)
    {
        if ((*it)->file->fileHandle)
        {
            request = *it;
            queueSavingRequests.erase(it);
            break;
        }
    }

    if (request == nullptr)
    {
        request = queueSavingRequests.front();
        queueSavingRequests.erase(queueSavingRequests.begin());
    }

    return request;
}

void IoThread::save(SavingThreadRequest* request)
{
    if (!request->file->fileHandle)
    {
        SWAG_ASSERT(!request->file->done);
        for (int tryOpen = 0; tryOpen < 10; tryOpen++)
        {
            // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
            // Without that, fopen can fail due to compiling processes
            if (request->file->firstSave)
                IoThread::openFile(&request->file->fileHandle, request->file->fileName.c_str(), "wtN", tryOpen == 9);
            else
                IoThread::openFile(&request->file->fileHandle, request->file->fileName.c_str(), "a+tN", tryOpen == 9);
            if (request->file->fileHandle)
            {
                //setvbuf(file, nullptr, _IONBF, 0);
                break;
            }

            Sleep(10);
        }
    }

    auto file = request->file->fileHandle;
    if (!file)
        return;

    request->file->firstSave = false;
    fwrite(request->buffer, 1, request->bufferSize, file);

    if (request->lastOne)
    {
        IoThread::closeFile(&request->file->fileHandle);
        request->file->notifySave(true);
    }
    else if (request->flush)
    {
        request->file->notifySave(request->lastOne);
    }

    releaseSavingRequest(request);
}

void IoThread::load(LoadingThreadRequest* request)
{
    request->loadedSize = 0;
    if (request->file->openRead())
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
