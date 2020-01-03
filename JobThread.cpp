#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "Context.h"
#include "Job.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Diagnostic.h"
#include "DiagnosticInfos.h"

JobThread::JobThread(int rank)
{
    thread = new std::thread(&JobThread::loop, this);
    OS::setThreadName(thread, "JobThread");

    //SetThreadAffinityMask(thread->native_handle(), (uint64_t) 1 << rank);
}

JobThread::~JobThread()
{
    delete thread;
}

void JobThread::notifyJob()
{
    lock_guard lk(mutexNotify);
    condVar.notify_all();
}

void JobThread::waitForANewJob()
{
    unique_lock lk(mutexNotify);
    condVar.wait(lk);
}

void JobThread::loop()
{
    // TLS context
    OS::tlsSetValue(g_tlsContextIdByteCode, &g_defaultContextByteCode);

    while (!requestEnd)
    {
        auto job = g_ThreadMgr.getJob(this);
        if (job == nullptr)
        {
            if (requestEnd)
                break;
            continue;
        }

        int exceptionCode = 0;
        g_ThreadMgr.executeOneJob(job, exceptionCode);

        // Job has raised an exception !
        if (exceptionCode)
        {
            g_diagnosticInfos.reportError(format("exception '%X' during job execution !", exceptionCode));
        }

#ifdef SWAG_HAS_ASSERT
        g_diagnosticInfos.clear();
#endif
    }
}