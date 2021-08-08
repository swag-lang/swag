#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "Context.h"

JobThread::JobThread(int rank)
{
    thread = new std::thread(&JobThread::loop, this);
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