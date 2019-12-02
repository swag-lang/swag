#include "pch.h"
#include "ThreadManager.h"
#include "LoadingThread.h"
#include "SavingThread.h"
#include "JobThread.h"
#include "Global.h"
#include "CommandLine.h"
#include "Stats.h"
#include "Job.h"
#include "Context.h"
#include "Module.h"

ThreadManager g_ThreadMgr;

void ThreadManager::init()
{
    initDefaultContext();

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
    unique_lock lk(mutexAdd);

    // Be sure Job::hasEnded has been called before
    // This shouldn't take a long time
    while (true)
    {
        unique_lock lk1(job->executeMutex);
        if (!(job->flags & JOB_IS_IN_THREAD))
            break;
    }

    // We have a new dependency job
    if (job->dependentModule)
    {
        unique_lock lk1(job->dependentModule->mutexDependency);
        job->dependentModule->waitOnJobs++;
    }

    job->flags &= ~JOB_IS_DEPENDENT;

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

bool ThreadManager::doneWithJobs()
{
    return queueJobs.empty() && processingJobs == 0;
}

void ThreadManager::jobHasEnded(Job* job)
{
    unique_lock lk(job->executeMutex);
	job->flags &= ~JOB_IS_IN_THREAD;

    // Wakeup build job for module if necessary
    if (job->dependentModule)
    {
        unique_lock lk1(job->dependentModule->mutexDependency);
		job->dependentModule->waitOnJobs--;
        if (job->dependentModule->waitOnJobs == 0)
            g_ThreadMgr.addJob(job->dependentModule->buildJob);
    }

    processingJobs--;
    unique_lock<mutex> lk1(mutexDone);
    if (doneWithJobs())
        condVarDone.notify_all();
}

void ThreadManager::waitEndJobs()
{
    while (true)
    {
        auto job = getJob();
        if (job)
        {
            auto result = job->execute();
            if (result == JobResult::ReleaseJob)
                job->doneJob();
            g_ThreadMgr.jobHasEnded(job);
            continue;
        }

        unique_lock<mutex> lk1(mutexDone);
        if (doneWithJobs())
            break;
        condVarDone.wait(lk1);
    }
}

Job* ThreadManager::getJob()
{
    unique_lock lk(mutexAdd);
    if (queueJobs.empty())
        return nullptr;
    auto job = queueJobs.back();
    queueJobs.pop_back();
    processingJobs++;
    SWAG_ASSERT(!(job->flags & JOB_IS_IN_THREAD));
    job->flags |= JOB_IS_IN_THREAD;
    return job;
}

Job* ThreadManager::getJob(JobThread* thread)
{
    auto job = getJob();

    if (job == nullptr)
    {
        {
            unique_lock lk(mutexAdd);
            availableThreads.push_back(thread);
        }

        thread->waitJob();
        return nullptr;
    }

    job->thread = thread;
    return job;
}

void ThreadManager::addPendingJob(Job* job)
{
    unique_lock lk(mutexAdd);
    pendingJobs.push_back(job);
    job->pendingIndex = (int) pendingJobs.size() - 1;
    job->thread       = nullptr;
}