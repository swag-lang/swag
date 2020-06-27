#include "pch.h"
#include "ThreadManager.h"
#include "JobThread.h"
#include "Stats.h"
#include "Job.h"
#include "Context.h"
#include "DiagnosticInfos.h"

ThreadManager g_ThreadMgr;

void ThreadManager::init()
{
    initDefaultContext();

    int numCores = std::thread::hardware_concurrency();
    if (g_CommandLine.numCores == 0)
        g_CommandLine.numCores = numCores;
    int numWorkers     = g_CommandLine.numCores;
    numWorkers         = max(1, numWorkers);
    numWorkers         = min(numWorkers, numCores);
    g_Stats.numWorkers = numWorkers;

    for (int i = 0; i < numWorkers; i++)
        workerThreads.push_back(new JobThread(i));
}

void ThreadManager::addJob(Job* job)
{
    unique_lock lk(mutexAdd);
    addJobNoLock(job);
}

void ThreadManager::addJobNoLock(Job* job)
{
    if (job->flags & JOB_IS_IN_THREAD)
    {
        job->flags |= JOB_IS_PENDING_RUN;
        return;
    }

    SWAG_ASSERT(!(job->flags & JOB_IS_IN_QUEUE));
    job->flags |= JOB_IS_IN_QUEUE;

    // We are a new dependency job (child of another job that depends on us)
    if (job->dependentJob)
    {
        if (!(job->flags & JOB_IS_PENDING))
        {
            job->flags |= JOB_IS_PENDING;
            job->dependentJob->waitOnJobs++;
        }
    }

    // Remove from waiting list
    if (job->waitingJobIndex != -1)
    {
        waitingJobs[job->waitingJobIndex]                  = waitingJobs.back();
        waitingJobs[job->waitingJobIndex]->waitingJobIndex = job->waitingJobIndex;
        waitingJobs.pop_back();
        job->waitingJobIndex = -1;
    }

    queueJobs.push_back(job);

    // Wakeup one thread
    if (!availableThreads.empty())
    {
        auto thread = availableThreads.back();
        availableThreads.pop_back();
        thread->notifyJob();
    }
}

void ThreadManager::jobHasEnded(Job* job, JobResult result)
{
    unique_lock lk(mutexAdd);

    SWAG_ASSERT(job->flags & JOB_IS_IN_THREAD);
    job->flags &= ~JOB_IS_IN_THREAD;
    jobsInThreads--;

    // Some jobs have been registered to be pushed
    for (auto toRun : job->jobsToAdd)
        addJobNoLock(toRun);
    job->jobsToAdd.clear();

    // A request has been made to run the job, and the job was not ended
    // So we rerun it immediately
    if (job->flags & JOB_IS_PENDING_RUN)
    {
        SWAG_ASSERT(result != JobResult::ReleaseJob);
        job->flags &= ~JOB_IS_PENDING_RUN;
        addJobNoLock(job);
        return;
    }

    // Some jobs need to be run because this one is finished
    if (result == JobResult::ReleaseJob)
    {
        SWAG_RACE_CONDITION_WRITE(job->dependentJobs.raceCondition);
        for (auto toRun : job->dependentJobs.list)
            addJobNoLock(toRun);
        job->dependentJobs.clear();
    }

    // Notify my parent job that i'm done
    if (result != JobResult::KeepJobAlivePending && job->dependentJob)
    {
        SWAG_ASSERT(job->dependentJob->waitOnJobs);
        job->dependentJob->waitOnJobs--;
        job->flags &= ~JOB_IS_PENDING;
        if (!job->dependentJob->waitOnJobs)
            g_ThreadMgr.addJobNoLock(job->dependentJob);
    }

    // We are not really done. Just keep a trace of us
    if (result != JobResult::ReleaseJob)
    {
        waitingJobs.push_back(job);
        job->waitingJobIndex = (int) waitingJobs.size() - 1;
    }

    // Is this the last job ?
    unique_lock lk1(mutexDone);
    if (doneWithJobs())
        condVarDone.notify_all();
}

static int exceptionHandler()
{
    g_diagnosticInfos.reportError("exception during job execution !");

    if (g_CommandLine.devMode)
    {
        SWAG_ASSERT(false);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

void ThreadManager::executeOneJob(Job* job)
{
    __try
    {
        auto result = job->execute();
        jobHasEnded(job, result);
        if (result == JobResult::ReleaseJob)
            job->release();
    }
    __except (exceptionHandler())
    {
    }
}

bool ThreadManager::doneWithJobs()
{
    return queueJobs.empty() && jobsInThreads == 0;
}

void ThreadManager::waitEndJobs()
{
    while (true)
    {
        unique_lock lk(mutexDone);
        if (doneWithJobs())
            break;
        condVarDone.wait(lk);
    }
}

Job* ThreadManager::getJob(uint32_t affinity, function<bool(Job*)> canGetJob)
{
    unique_lock lk(mutexAdd);
    if (queueJobs.empty())
        return nullptr;

    auto job = queueJobs.back();
    if (!(job->affinity & affinity))
        return nullptr;
    if (canGetJob && !canGetJob(job))
        return nullptr;

    queueJobs.pop_back();

    SWAG_ASSERT(job->flags & JOB_IS_IN_QUEUE);
    job->flags &= ~JOB_IS_IN_QUEUE;
    SWAG_ASSERT(!(job->flags & JOB_IS_IN_THREAD));
    job->flags |= JOB_IS_IN_THREAD;

    jobsInThreads++;
    return job;
}

Job* ThreadManager::getJob(JobThread* thread)
{
    auto job = getJob(AFFINITY_ALL);
    if (job)
        return job;

    mutexAdd.lock();
    availableThreads.push_back(thread);
    mutexAdd.unlock();

    thread->waitForANewJob();
    return nullptr;
}

void ThreadManager::participate(mutex& lock, uint32_t affinity, const function<bool(Job*)>& canGetJob)
{
    while (true)
    {
        if (lock.try_lock())
            return;

        auto job = getJob(affinity, canGetJob);
        if (!job)
            continue;
        g_ThreadMgr.executeOneJob(job);
    }
}

void ThreadManager::participate(const function<bool(Job*)>& canGetJob)
{
    auto job = getJob(AFFINITY_ALL, canGetJob);
    if (!job)
        return;

    g_ThreadMgr.executeOneJob(job);
}