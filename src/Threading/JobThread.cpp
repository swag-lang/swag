#include "pch.h"
#include "Threading/JobThread.h"
#include "Backend/Context.h"
#include "Threading/ThreadManager.h"

thread_local uint32_t g_ThreadIndex = 0;

JobThread::JobThread(uint32_t rank)
{
    threadRank = rank;
    thread     = new std::thread(&JobThread::loop, this);
    OS::setThreadName(thread, "JobThread");
}

JobThread::~JobThread()
{
    delete thread;
}

void JobThread::notifyJob()
{
    std::lock_guard lk(mutexNotify);
    condVar.notify_one();
}

void JobThread::loop()
{
    OS::tlsSetValue(g_TlsContextId, &g_DefaultContext);
    g_ThreadIndex = threadRank;

    while (!requestEnd)
    {
        const auto job = g_ThreadMgr.getJob(this);
        if (job == nullptr)
        {
            if (requestEnd)
                break;
            continue;
        }

        g_ThreadMgr.executeOneJob(job);
    }
}
