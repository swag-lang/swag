#include "pch.h"
#include "ThreadManager.h"
#include "JobThread.h"
#include "Context.h"
#include "Module.h"
#include "AstNode.h"
#include "ByteCodeStack.h"
#include "Workspace.h"

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

    queueJobs.affinity.resize(g_CommandLine.numCores);
    queueJobsIO.affinity.resize(g_CommandLine.numCores);
    queueJobsOpt.affinity.resize(g_CommandLine.numCores);

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
    ScopedLock lk(mutexAdd);
    addJobNoLock(job);
}

void ThreadManager::addJobNoLock(Job* job)
{
    if (doJobCount)
        addJobCount++;

    if (job->flags & JOB_IS_IN_THREAD)
    {
        SWAG_ASSERT(job->waitOnJobs == 0);
        job->flags |= JOB_IS_PENDING_RUN;
        return;
    }

    // This should not happen... but this can happen
    // A thread is added, but should not run because a dependency is still running
    // So, yes, this is an anti bug...
    if (job->waitOnJobs != 0 && !(job->flags & JOB_ACCEPT_PENDING_COUNT))
        return;

    SWAG_ASSERT(!(job->flags & JOB_IS_IN_QUEUE));
    job->flags |= JOB_IS_IN_QUEUE;

    // We are a new dependency job (child of another job that depends on us)
    if (job->dependentJob)
    {
        if (!(job->flags & JOB_IS_PENDING))
        {
            job->flags |= JOB_IS_PENDING;
            job->dependentJob->waitOnJobs++;
            job->dependentJob->flags &= ~JOB_IS_PENDING_RUN;
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

    SWAG_ASSERT(job->waitOnJobs == 0 || (job->flags & JOB_ACCEPT_PENDING_COUNT));
    job->flags &= ~JOB_ACCEPT_PENDING_COUNT;

    if (job->flags & JOB_IS_OPT)
    {
        if (job->affinity != UINT32_MAX)
        {
            queueJobsOpt.affinity[job->affinity].push_back(job);
            queueJobsOpt.affinityCount++;
        }
        else
            queueJobsOpt.jobs.push_back(job);
    }
    else if (job->flags & JOB_IS_IO)
    {
        if (job->affinity != UINT32_MAX)
        {
            queueJobsIO.affinity[job->affinity].push_back(job);
            queueJobsIO.affinityCount++;
        }
        else
            queueJobsIO.jobs.push_back(job);
    }
    else
    {
        if (job->affinity != UINT32_MAX)
        {
            queueJobs.affinity[job->affinity].push_back(job);
            queueJobs.affinityCount++;
        }
        else
            queueJobs.jobs.push_back(job);
    }

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
    ScopedLock lk(mutexAdd);
    ScopedLock lk1(mutexDone);

    if (doJobCount)
        addJobCount--;

    SWAG_ASSERT(job->flags & JOB_IS_IN_THREAD);
    job->flags &= ~JOB_IS_IN_THREAD;
    if (job->flags & JOB_IS_OPT)
    {
        SWAG_ASSERT(jobsOptInThreads);
        jobsOptInThreads--;
    }
    else
    {
        SWAG_ASSERT(jobsInThreads);
        jobsInThreads--;
    }

    job->wakeUpBy = nullptr;

    // Some jobs have been registered to be pushed
    for (auto toRun : job->jobsToAdd)
    {
        toRun->wakeUpBy = job;
        addJobNoLock(toRun);
    }

    job->jobsToAdd.clear();

    // A request has been made to run the job, and the job was not ended
    // So we rerun it immediately
    if (job->flags & JOB_IS_PENDING_RUN)
    {
        job->flags &= ~JOB_IS_PENDING_RUN;
        if (result != JobResult::ReleaseJob)
        {
            addJobNoLock(job);
            return;
        }
    }

    // Some jobs need to be run because this one is finished
    if (result == JobResult::ReleaseJob)
    {
        ScopedLock lk2(job->mutexDependent);
        for (auto toRun : job->dependentJobs.list)
        {
            toRun->wakeUpBy = job;
            addJobNoLock(toRun);
        }

        job->dependentJobs.clear();
    }

    // If i am an IO job, then decrease counter
    if (job->flags & JOB_IS_IO)
    {
        SWAG_ASSERT(currentJobsIO);
        currentJobsIO--;
    }

    // Do we need to wakeup my parent job ?
    bool wakeUpParent = false;

    // Only if the job is done
    if (result != JobResult::KeepJobAlive)
        wakeUpParent = true;

    // or if we are waiting for a placeholder symbol to be solved, because the parent job *will* generate the placeholder
    else if (job->waitingSymbolSolved && job->waitingSymbolSolved->kind == SymbolKind::PlaceHolder)
        wakeUpParent = true;

    // or if the symbol is waiting for code generation
    else if (job->waitingSymbolSolved && job->waitingSymbolSolved->flags & SYMBOL_ATTRIBUTE_GEN)
        wakeUpParent = true;

    // Notify my parent job that i am done
    if (wakeUpParent && job->dependentJob)
    {
        SWAG_ASSERT(job->dependentJob->waitOnJobs);
        job->dependentJob->waitOnJobs--;
        job->flags &= ~JOB_IS_PENDING;
        if (!job->dependentJob->waitOnJobs)
        {
            job->dependentJob->wakeUpBy = job;
            addJobNoLock(job->dependentJob);
        }
    }

    // We are not really done. Just keep a trace of us
    if (result != JobResult::ReleaseJob)
    {
        waitingJobs.push_back(job);
        job->waitingJobIndex = (int) waitingJobs.size() - 1;
    }
    else if (job->jobGroup)
    {
        job->jobGroup->doneJob();
    }

    // Is this the last job ?
    if (doneWithJobs())
        condVarDone.notify_all();
}

#if !defined(SWAG_DEV_MODE)
static void exceptionMessage(Job* job, SWAG_LPEXCEPTION_POINTERS args)
{
    g_Log.lock();
    g_Log.setColor(LogColor::Red);
    g_Log.print("fatal error: hardware exception during job execution !\n");

    g_Log.setColor(LogColor::White);
    g_Log.messageHeaderDot("exception code", Fmt("0x%08X\n", args->ExceptionRecord->ExceptionCode), LogColor::White, LogColor::White, " ", false);
    if (job->baseContext && job->baseContext->sourceFile)
    {
        g_Log.messageHeaderDot("current source file", job->baseContext->sourceFile->name, LogColor::White, LogColor::White, " ", false);
        g_Log.messageHeaderDot("current module", job->baseContext->sourceFile->module->name, LogColor::White, LogColor::White, " ", false);
    }

    if (job->nodes.size())
    {
        auto node = job->nodes.back();
        g_Log.messageHeaderDot("current node", node->token.text, LogColor::White, LogColor::White, " ", false);
        if (node->sourceFile)
            g_Log.messageHeaderDot("current location", Fmt("%s:%d:%d", node->sourceFile->path.c_str(), node->token.startLocation.line + 1, node->token.startLocation.column + 1), LogColor::White, LogColor::White, " ", false);
    }

    g_Log.unlock();
}

static int exceptionHandler(Job* job, SWAG_LPEXCEPTION_POINTERS args)
{
    if (OS::isDebuggerAttached())
    {
        OS::errorBox("[Developer Mode]", "Exception raised !");
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    exceptionMessage(job, args);
    return SWAG_EXCEPTION_EXECUTE_HANDLER;
}

#else
static int exceptionHandler(Job* job, SWAG_LPEXCEPTION_POINTERS args)
{
    g_ByteCodeStackTrace->reportError("exception during job execution !");
    OS::errorBox("[Developer Mode]", "Exception raised !");
    return EXCEPTION_CONTINUE_EXECUTION;
}
#endif

void ThreadManager::executeOneJob(Job* job)
{
    SWAG_TRY
    {
        auto result = job->execute();
        jobHasEnded(job, result);
        if (result == JobResult::ReleaseJob)
            job->release();
    }
    SWAG_EXCEPT(exceptionHandler(job, SWAG_GET_EXCEPTION_INFOS()))
    {
        OS::exit(-1);
    }
}

bool ThreadManager::doneWithJobs()
{
    return queueJobs.jobs.empty() &&
           queueJobsIO.jobs.empty() &&
           queueJobs.affinityCount == 0 &&
           queueJobsIO.affinityCount == 0 &&
           jobsInThreads == 0;
}

void ThreadManager::clearOptionalJobs()
{
    {
        ScopedLock lk(mutexAdd);

        for (auto p : queueJobsOpt.jobs)
        {
            p->release();
        }
        queueJobsOpt.jobs.clear();

        for (int i = 0; i < g_CommandLine.numCores; i++)
        {
            for (auto p : queueJobsOpt.affinity[i])
                p->release();
            queueJobsOpt.affinity[i].clear();
        }
        queueJobsOpt.affinityCount = 0;
    }

    while (jobsOptInThreads)
        this_thread::yield();
}

void ThreadManager::waitEndJobs()
{
    // If one core only, do the jobs right now, in order
    if (g_CommandLine.numCores == 1)
    {
        while (tryExecuteJob()) {}
        return;
    }

    // Else wait for all jobs to be done in other threads
    while (true)
    {
        unique_lock lk(mutexDone);
        if (doneWithJobs())
            break;
        condVarDone.wait(lk);
    }
}

Job* ThreadManager::getJob()
{
    {
        ScopedLock lk(mutexAdd);
        auto       job = getJobNoLock();
        if (job)
            return job;
    }

    // Otherwise, steal a syntax job from a module if we can
    SharedLock lk(g_Workspace->mutexModules);
    for (auto p : g_Workspace->modules)
    {
        auto job = p->syntaxGroup.pickJob();
        if (job)
        {
            SWAG_ASSERT(job->flags & JOB_IS_OPT);
            eatJob(job);
            return job;
        }
    }

    return nullptr;
}

Job* ThreadManager::getJobNoLock()
{
    Job* job;

    // If no IO is running, then we can take one in priority
    if (!currentJobsIO)
    {
        job = getJobNoLock(queueJobsIO);
        if (job)
        {
            currentJobsIO++;
            return job;
        }
    }

    // Else we take a normal job if we can
    job = getJobNoLock(queueJobs);
    if (job)
        return job;

    // Else we take an optional job if we can
    job = getJobNoLock(queueJobsOpt);
    if (job)
        return job;

    // Otherwise, then IO, as there's nothing left
    job = getJobNoLock(queueJobsIO);
    if (job)
    {
        currentJobsIO++;
        return job;
    }

    return nullptr;
}

Job* ThreadManager::getJobNoLock(JobQueue& queue)
{
    bool jEmpty = queue.jobs.empty();
    if (jEmpty && !queue.affinityCount)
        return nullptr;

    ScopedLock lk(mutexDone);
    Job*       job;

    if (!queue.affinity[g_ThreadIndex].empty())
    {
        job = queue.affinity[g_ThreadIndex].get_pop_back();
        SWAG_ASSERT(queue.affinityCount);
        queue.affinityCount--;
    }
    else if (jEmpty)
    {
        job = nullptr;
        for (int i = 0; i < queue.affinity.size(); i++)
        {
            if (!queue.affinity[i].empty())
            {
                job = queue.affinity[i].get_pop_back();
                SWAG_ASSERT(queue.affinityCount);
                queue.affinityCount--;
                break;
            }
        }
    }
    else
    {
        auto jobPickIndex = (int) queue.jobs.size() - 1;
#ifdef SWAG_DEV_MODE
        if (g_CommandLine.randomize)
            jobPickIndex = rand() % queue.jobs.count;
#endif
        job = queue.jobs[jobPickIndex];
        queue.jobs.erase(jobPickIndex);
    }

    SWAG_ASSERT(job);
    SWAG_ASSERT(job->flags & JOB_IS_IN_QUEUE);
    job->flags &= ~JOB_IS_IN_QUEUE;
    eatJob(job);

    return job;
}

void ThreadManager::eatJob(Job* job)
{
    SWAG_ASSERT(!(job->flags & JOB_IS_IN_THREAD));
    job->flags |= JOB_IS_IN_THREAD;

    job->waitingKind = JobWaitKind::None;
    job->jobThread   = nullptr;

    if (job->flags & JOB_IS_OPT)
        jobsOptInThreads++;
    else
        jobsInThreads++;
}

Job* ThreadManager::getJob(JobThread* thread)
{
    auto job = getJob();
    if (job)
    {
        job->jobThread = thread;
        return job;
    }

    unique_lock lk(thread->mutexNotify);

    {
        ScopedLock lk1(mutexAdd);

        // Try another time in case another thread has pushed a job
        job = getJobNoLock();
        if (job)
        {
            job->jobThread = thread;
            return job;
        }

        availableThreads.push_back(thread);
    }

    thread->condVar.wait(lk);
    return nullptr;
}

bool ThreadManager::tryExecuteJob()
{
    auto job = getJob();
    if (!job)
        return false;
    executeOneJob(job);
    return true;
}