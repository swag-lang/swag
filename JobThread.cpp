#include "pch.h"
#include "JobThread.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "Global.h"
#include "Job.h"
#include "Log.h"
#include "Context.h"
#include "Os.h"

JobThread::JobThread()
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

void JobThread::waitJob()
{
    unique_lock lk(mutexNotify);
    condVar.wait(lk);
}

void JobThread::loop()
{
    // TLS context
    OS::tlsSetValue(g_tlsContextIdByteCode, &g_defaultContextByteCode);
    OS::tlsSetValue(g_tlsContextIdBackend, &g_defaultContextBackend);

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

#ifdef SWAG_HAS_ASSERT
        g_diagnosticInfos.clear();
#endif
    }
}