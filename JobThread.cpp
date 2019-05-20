#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "Global.h"

JobThread::JobThread()
{
    thread = new std::thread(&JobThread::loop, this);
}

JobThread::~JobThread()
{
    delete thread;
}

void JobThread::notifyJob()
{
    lock_guard<mutex> lk(mutexNotify);
    condVar.notify_one();
}

void JobThread::waitJob()
{
    unique_lock<mutex> lk(mutexNotify);
    condVar.wait(lk);
}

void JobThread::loop()
{
    while (!requestEnd)
    {
        auto job = g_ThreadMgr.getJob(this);
        if (job == nullptr)
        {
            if (requestEnd)
                break;
            continue;
        }

        job->execute();
        job->release();
        g_ThreadMgr.jobHasEnded();
    }
}