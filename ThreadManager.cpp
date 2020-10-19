#include "pch.h"
#include "ThreadManager.h"
#include "JobThread.h"
#include "Stats.h"
#include "Job.h"
#include "Context.h"
#include "ByteCodeStack.h"
#include "SymTable.h"

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

    // When numWorkers is 1, then we do not want any worker thread. The main thread
    // will execute jobs by its own (otherwise the main thread will be suspended, and workers
    // do their job)
    if (numWorkers > 1)
    {
        for (int i = 0; i < numWorkers; i++)
            workerThreads.push_back(new JobThread(i));
    }
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

    if (job->flags & JOB_IS_IO)
        queueJobsIO.push_back(job);
    else
        queueJobs.push_back(job);

    // Wakeup one thread
    if (g_CommandLine.numCores != 1)
    {
        if (!availableThreads.empty())
        {
            auto thread = availableThreads.back();
            availableThreads.pop_back();
            thread->notifyJob();
        }
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
        job->flags &= ~JOB_IS_PENDING_RUN;
        if (result != JobResult::ReleaseJob)
        {
            job->flags &= ~JOB_IS_PENDING_RUN;
            addJobNoLock(job);
            return;
        }
    }

    // Some jobs need to be run because this one is finished
    if (result == JobResult::ReleaseJob)
    {
        scoped_lock lk1(job->mutexDependent);
        for (auto toRun : job->dependentJobs.list)
            addJobNoLock(toRun);
        job->dependentJobs.clear();
    }

    // If i am an IO job, then decrease counter
    if (job->flags & JOB_IS_IO)
    {
        SWAG_ASSERT(currentJobsIO);
        currentJobsIO--;
    }

    // Do we need to wakeup my parent job ?
    // Only if the job is done, or if we are waiting for a placeholder symbol to be solved, because
    // the parent job *will* generate the placeholder
    bool wakeUpParent = false;
    if (result != JobResult::KeepJobAlive || (job->waitingSymbolSolved && job->waitingSymbolSolved->kind == SymbolKind::PlaceHolder))
        wakeUpParent = true;

    // Notify my parent job that i am done
    if (wakeUpParent && job->dependentJob)
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
    if (g_CommandLine.devMode)
    {
        g_byteCodeStack.reportError("exception during job execution !");
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
    if (g_CommandLine.numCores == 1)
    {
        while (participate(AFFINITY_ALL))
            ;
    }
    else
    {
        while (true)
        {
            unique_lock lk(mutexDone);
            if (doneWithJobs())
                break;
            condVarDone.wait(lk);
        }
    }
}

Job* ThreadManager::getJob(uint32_t affinity)
{
    unique_lock lk(mutexAdd);
    Job*        job;

    // If no IO is running, then we can take one in priority
    if (!currentJobsIO && (affinity & AFFINITY_IO))
    {
        job = getJob(affinity, queueJobsIO);
        if (job)
        {
            currentJobsIO++;
            return job;
        }
    }

    // Else we take a normal job if we can
    job = getJob(affinity, queueJobs);
    if (job)
        return job;

    // Otherwise, then IO, as there's nothing left
    if (affinity & AFFINITY_IO)
    {
        job = getJob(affinity, queueJobsIO);
        if (job)
            currentJobsIO++;
    }

    return job;
}

#include "SourceFile.h"
Job* ThreadManager::getJob(uint32_t affinity, VectorNative<Job*>& queue)
{
    if (queue.empty())
        return nullptr;

    Job* job;
    int  jobPickIndex = -1;

    if (g_CommandLine.randomize)
        jobPickIndex = rand() % queue.count;
    else
        jobPickIndex = (int) queue.size() - 1;

    job = queue[jobPickIndex];
    if (!(job->affinity & affinity))
        return nullptr;

    queue.erase(jobPickIndex);

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

void ThreadManager::participate(mutex& lock, uint32_t affinity)
{
    while (true)
    {
        if (lock.try_lock())
            return;

        auto job = getJob(affinity);
        if (!job)
            continue;
        g_ThreadMgr.executeOneJob(job);
    }
}

bool ThreadManager::participate(uint32_t affinity)
{
    auto job = getJob(affinity);
    if (!job)
        return false;
    g_ThreadMgr.executeOneJob(job);
    return true;
}