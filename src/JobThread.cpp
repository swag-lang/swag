#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "Context.h"

thread_local int g_ThreadIndex = 0;

JobThread::JobThread(int rank)
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
    lock_guard lk(mutexNotify);
    condVar.notify_one();
}

void JobThread::loop()
{
    // TLS context
    OS::tlsSetValue(g_TlsContextId, &g_DefaultContext);
    g_CompilerAllocTh += 1;
    g_ThreadIndex = threadRank;

    while (!requestEnd)
    {
        auto job = g_ThreadMgr.getJob(this);
        if (job == nullptr)
        {
            if (requestEnd)
                break;
            continue;
        }

        g_ThreadMgr.executeOneJob(job);
    }
}