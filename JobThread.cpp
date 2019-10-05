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
    lock_guard<mutex> lk(mutexNotify);
    condVar.notify_one();
}

void JobThread::waitJob()
{
    unique_lock<mutex> lk(mutexNotify);
    condVar.wait(lk);
}

bool JobThread::executeJob(Job* job, string& exception)
{
    __try
    {
        auto result = job->execute();
        if (result == JobResult::ReleaseJob)
            job->release();
        g_ThreadMgr.jobHasEnded();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        exception = "executeJob, unhandled exception";
        return false;
    }

    return true;
}

void JobThread::loop()
{
	// TLS context
	TlsSetValue(g_tlsContextId, &g_defaultContext);

    while (!requestEnd)
    {
        auto job = g_ThreadMgr.getJob(this);
        if (job == nullptr)
        {
            if (requestEnd)
                break;
            continue;
        }

        string exception;
        if (!executeJob(job, exception))
            g_Log.error(exception);

#ifdef SWAG_HAS_ASSERT
        g_diagnosticInfos.clear();
#endif
    }
}