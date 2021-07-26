#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "Context.h"
#include "Job.h"
#include "SourceFile.h"
#include "AstNode.h"
#include "Diagnostic.h"

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
    OS::tlsSetValue(g_tlsContextId, &g_defaultContext);
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