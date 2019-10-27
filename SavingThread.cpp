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
    if (!queueRequests.empty())
        return;
    condVar.wait(lk);
}

std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR  messageBuffer = nullptr;
    size_t size          = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
                                 errorMessageID,
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 (LPSTR) &messageBuffer,
                                 0,
                                 NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
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
            // Seems that we need 'N' flag to avoid handle to be shared with spawned processes
            // Without that, fopen can fail due to compiling processes
			if(req->firstSave)
				fopen_s(&file, req->file->fileName.c_str(), "wtN");
			else
				fopen_s(&file, req->file->fileName.c_str(), "a+tN");
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
            req->ioError = true;
        }

        req->file->notifySave(req);
    }
}
