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
    if (g_CommandLine->numCores == 0)
        g_CommandLine->numCores = numCores;
    int numWorkers     = g_CommandLine->numCores;
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
    ScopedLock lk(mutexAdd);
    addJobNoLock(job);
}

void ThreadManager::addJobNoLock(Job* job)
{
    if (job->flags & JOB_IS_IN_THREAD)
    {
        SWAG_ASSERT(job->waitOnJobs == 0);
        job->flags |= JOB_IS_PENDING_RUN;
        return;
    }

    // This should not happen... but this can happen
    // A thread is added, but should not run because a dependency is still running
    // So, yes, this is an anti bug...
    if (job->waitOnJobs != 0)
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

    SWAG_ASSERT(job->waitOnJobs == 0);

    if (job->flags & JOB_IS_OPT)
        queueJobsOpt.push_back(job);
    else if (job->flags & JOB_IS_IO)
        queueJobsIO.push_back(job);
    else
        queueJobs.push_back(job);

    // Wakeup one thread
    if (g_CommandLine->numCores != 1)
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

    SWAG_ASSERT(job->flags & JOB_IS_IN_THREAD);
    job->flags &= ~JOB_IS_IN_THREAD;
    if (job->flags & JOB_IS_OPT)
        jobsOptInThreads--;
    else
        jobsInThreads--;
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
        ScopedLock lk1(job->mutexDependent);
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
    {
        if (job->dependentJob)
            job->dependentJob->flags |= JOB_PENDING_PLACE_HOLDER;
        wakeUpParent = true;
    }

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
    ScopedLock lk1(mutexDone);
    if (doneWithJobs())
        condVarDone.notify_all();
}

#ifndef SWAG_DEV_MODE
static void exceptionMessage(Job* job, LPEXCEPTION_POINTERS args)
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
#endif

static int exceptionHandler(Job* job, LPEXCEPTION_POINTERS args)
{
#ifdef SWAG_DEV_MODE
    g_ByteCodeStack.reportError("exception during job execution !");
    OS::errorBox("[Developer Mode]", "Exception raised !");
    return EXCEPTION_CONTINUE_EXECUTION;
#else
    exceptionMessage(job, args);
    return EXCEPTION_EXECUTE_HANDLER;
#endif
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
    __except (exceptionHandler(job, GetExceptionInformation()))
    {
        OS::exit(-1);
    }
}

bool ThreadManager::doneWithJobs()
{
    return queueJobs.empty() && queueJobsIO.empty() && jobsInThreads == 0;
}

void ThreadManager::clearOptionalJobs()
{
    {
        ScopedLock lk(mutexAdd);
        for (auto p : queueJobsOpt)
            p->release();
        queueJobsOpt.clear();
    }

    while (jobsOptInThreads)
        this_thread::yield();
}

void ThreadManager::waitEndJobs()
{
    // If one core only, do the jobs right now, in order
    if (g_CommandLine->numCores == 1)
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
            job->flags |= JOB_IS_IN_THREAD;
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

Job* ThreadManager::getJobNoLock(VectorNative<Job*>& queue)
{
    if (queue.empty())
        return nullptr;

    auto jobPickIndex = (int) queue.size() - 1;
#ifdef SWAG_DEV_MODE
    if (g_CommandLine->randomize)
        jobPickIndex = rand() % queue.count;
#endif

    auto job = queue[jobPickIndex];
    queue.erase(jobPickIndex);

    SWAG_ASSERT(job->flags & JOB_IS_IN_QUEUE);
    job->flags &= ~JOB_IS_IN_QUEUE;
    SWAG_ASSERT(!(job->flags & JOB_IS_IN_THREAD));
    job->flags |= JOB_IS_IN_THREAD;

    job->waitingKind = JobWaitKind::None;
    job->jobThread   = nullptr;

    if (job->flags & JOB_IS_OPT)
        jobsOptInThreads++;
    else
        jobsInThreads++;

    return job;
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