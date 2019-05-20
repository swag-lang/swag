#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "JobThread.h"
#include "Global.h"

void ThreadManager::init()
{
    loadingThread = new LoadingThread();

    int numWorkers = g_Global.numCores - 2;
    numWorkers     = max(1, numWorkers);
    for (int i = 0; i < numWorkers; i++)
        workerThreads.push_back(new JobThread());
}

ThreadManager::~ThreadManager()
{
    delete loadingThread;
}

void ThreadManager::addJob(Job* job)
{
    lock_guard<mutex> lk(mutexAdd);
    queueJobs.push_back(job);
    if (availableThreads.empty())
        return;
    auto thread = availableThreads.back();
    availableThreads.pop_back();
    thread->notifyJob();
}

Job* ThreadManager::getJob()
{
    lock_guard<mutex> lk(mutexAdd);
    if (queueJobs.empty())
        return nullptr;
    auto job = queueJobs.back();
    queueJobs.pop_back();
    pendingJobs++;
    return job;
}

void ThreadManager::jobHasEnded()
{
    lock_guard<mutex> lk(mutexAdd);
    pendingJobs--;
    if (queueJobs.empty() && pendingJobs == 0)
        condVarDone.notify_all();
}

void ThreadManager::waitEndJobs()
{
    while (true)
    {
        {
            unique_lock<mutex> lk(mutexDone);
            condVarDone.wait(lk);
        }

        {
            lock_guard<mutex> lk(mutexAdd);
            if (queueJobs.empty())
                break;
        }
    }
}

Job* ThreadManager::getJob(JobThread* thread)
{
    auto job = getJob();
    if (job == nullptr)
    {
        mutexAdd.lock();
        availableThreads.push_back(thread);
        mutexAdd.unlock();
        thread->waitJob();
        return nullptr;
    }

    job->thread = thread;
    return job;
}
