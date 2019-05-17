#include "pch.h"
#include "JobThread.h"
#include "ThreadManager.h"
#include "Global.h"

JobThread::JobThread()
{
    m_thread = new std::thread(&JobThread::loop, this);
}

JobThread::~JobThread()
{
    delete m_thread;
}

void JobThread::notifyJob()
{
    lock_guard<mutex> lk(m_mutexNotify);
    m_Cv.notify_one();
}

void JobThread::waitJob()
{
    unique_lock<mutex> lk(m_mutexNotify);
    m_Cv.wait(lk);
}

void JobThread::loop()
{
    while (!m_requestEnd)
    {
        auto job = g_ThreadMgr.getJob(this);
        if (job == nullptr)
        {
            if (m_requestEnd)
                break;
            continue;
        }

        job->execute();
        job->release();
        g_ThreadMgr.jobHasEnded();
    }
}