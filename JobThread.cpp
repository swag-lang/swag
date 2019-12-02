#include "pch.h"
#include "JobThread.h"
#include "SourceFile.h"
#include "ThreadManager.h"
#include "Global.h"
#include "Job.h"
#include "Log.h"
#include "Context.h"

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
    lock_guard lk(mutexNotify);
    condVar.notify_one();
}

void JobThread::waitJob()
{
    unique_lock lk(mutexNotify);
    condVar.wait(lk);
}

bool JobThread::executeJob(Job* job, bool& exception)
{
    //__try
    {
        auto result = job->execute();
        if (result == JobResult::ReleaseJob)
        {
            job->doneJob();
        }

        g_ThreadMgr.jobHasEnded();
    }
    /*__except (EXCEPTION_EXECUTE_HANDLER)
    {
        exception = true;
        return false;
    }*/

    return true;
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

        bool exception = false;
        if (!executeJob(job, exception))
        {
            g_Log.error("executeJob, unhandled exception");
        }

#ifdef SWAG_HAS_ASSERT
        g_diagnosticInfos.clear();
#endif
    }
}