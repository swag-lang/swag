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
	scoped_lock<mutex> lk(mutexAdd);
    queueJobs.push_back(job);
    if (availableThreads.empty())
        return;
    auto thread = availableThreads.back();
    availableThreads.pop_back();
    thread->notifyJob();
}

Job* ThreadManager::getJob()
{
	scoped_lock<mutex> lk(mutexAdd);
    if (queueJobs.empty())
        return nullptr;
    auto job = queueJobs.back();
    queueJobs.pop_back();
    pendingJobs++;
    return job;
}

bool ThreadManager::doneWithJobs()
{
    return queueJobs.empty() && pendingJobs == 0;
}

void ThreadManager::jobHasEnded()
{
    scoped_lock<mutex> lk(mutexAdd);
    pendingJobs--;
    if (doneWithJobs())
    {
        unique_lock<mutex> lk1(mutexDone);
        condVarDone.notify_all();
    }
}

void ThreadManager::waitEndJobs()
{
    while (true)
    {
        unique_lock<mutex> lk1(mutexDone);
        {
            scoped_lock<mutex> lk(mutexAdd);
            if (doneWithJobs())
                break;
        }
        condVarDone.wait(lk1);
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
