#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "SavingThread.h"
#include "JobThread.h"
#include "Global.h"
#include "CommandLine.h"
#include "Stats.h"
#include "Job.h"

ThreadManager g_ThreadMgr;

void ThreadManager::init()
{
    loadingThread = new LoadingThread();
    savingThread  = new SavingThread();

    int numCores = std::thread::hardware_concurrency();
    if (g_CommandLine.numCores == 0)
        g_CommandLine.numCores = numCores - 2;
    int numWorkers     = g_CommandLine.numCores;
    numWorkers         = max(1, numWorkers);
    numWorkers         = min(numWorkers, numCores);
    g_Stats.numWorkers = numWorkers;

    for (int i = 0; i < numWorkers; i++)
        workerThreads.push_back(new JobThread());
}

ThreadManager::~ThreadManager()
{
    delete loadingThread;
    delete savingThread;
}

void ThreadManager::addJob(Job* job)
{
    scoped_lock<mutex> lk(mutexAdd);

    // Remove from pending list
    if (job->pendingIndex != -1)
    {
        pendingJobs[job->pendingIndex]               = pendingJobs.back();
        pendingJobs[job->pendingIndex]->pendingIndex = job->pendingIndex;
        pendingJobs.pop_back();
        job->pendingIndex = -1;
    }

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
    processingJobs++;
    return job;
}

bool ThreadManager::doneWithJobs()
{
    return queueJobs.empty() && processingJobs == 0;
}

void ThreadManager::jobHasEnded()
{
    processingJobs--;
    unique_lock<mutex> lk1(mutexDone);
    if (doneWithJobs())
        condVarDone.notify_all();
}

void ThreadManager::waitEndJobs()
{
    while (true)
    {
        unique_lock<mutex> lk1(mutexDone);
        if (doneWithJobs())
            break;
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

void ThreadManager::addPendingJob(Job* job)
{
    scoped_lock<mutex> lk(mutexAdd);
    pendingJobs.push_back(job);
    job->pendingIndex = (int) pendingJobs.size() - 1;
}