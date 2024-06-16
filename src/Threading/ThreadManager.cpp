#include "pch.h"
#include "Backend/Context.h"
#include "Syntax/AstNode.h"
#include "Threading/JobThread.h"
#include "Threading/ThreadManager.h"
#include "Wmf/Module.h"
#include "Wmf/Workspace.h"
#if !defined(SWAG_DEV_MODE)
#include "Report/Log.h"
#endif

ThreadManager g_ThreadMgr;

void ThreadManager::init()
{
    initDefaultContext();

    numWorkers = g_CommandLine.numCores == 0 ? std::thread::hardware_concurrency() : g_CommandLine.numCores;
    numWorkers = max(1, numWorkers);

    queueJobs.affinity.resize(g_ThreadMgr.numWorkers);
    queueJobsIO.affinity.resize(g_ThreadMgr.numWorkers);
    queueJobsOpt.affinity.resize(g_ThreadMgr.numWorkers);

    // When numWorkers is 1, then we do not want any worker thread. The main thread
    // will execute jobs by its own (otherwise the main thread will be suspended, and workers
    // do their job)
    if (numWorkers > 1)
    {
        for (uint32_t i = 0; i < numWorkers; i++)
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
    if (debuggerMode)
        job->addFlag(JOB_IS_DEBUGGER);

    if (job->hasFlag(JOB_IS_IN_THREAD))
    {
        SWAG_ASSERT(job->waitOnJobs == 0);
        job->addFlag(JOB_IS_PENDING_RUN);
        return;
    }

    // This should not happen... but this can happen
    // A thread is added, but should not run because a dependency is still running
    // So, yes, this is a hack...
    if (job->waitOnJobs != 0 && !job->hasFlag(JOB_ACCEPT_PENDING_COUNT))
        return;

    SWAG_ASSERT(!job->hasFlag(JOB_IS_IN_QUEUE));
    job->addFlag(JOB_IS_IN_QUEUE);

    // We are a new dependency job (child of another job that depends on us)
    if (job->dependentJob)
    {
        if (!job->hasFlag(JOB_IS_PENDING))
        {
            job->addFlag(JOB_IS_PENDING);
            job->dependentJob->waitOnJobs++;
            job->dependentJob->removeFlag(JOB_IS_PENDING_RUN);
        }
    }

    // Remove from waiting list
    if (job->waitingJobIndex != UINT32_MAX)
    {
        waitingJobs[job->waitingJobIndex]                  = waitingJobs.back();
        waitingJobs[job->waitingJobIndex]->waitingJobIndex = job->waitingJobIndex;
        waitingJobs.pop_back();
        job->waitingJobIndex = UINT32_MAX;
    }

    SWAG_ASSERT(job->waitOnJobs == 0 || job->hasFlag(JOB_ACCEPT_PENDING_COUNT));
    job->removeFlag(JOB_ACCEPT_PENDING_COUNT);

    if (job->hasFlag(JOB_IS_OPT))
    {
        if (job->affinity != UINT32_MAX)
        {
            queueJobsOpt.affinity[job->affinity].push_back(job);
            queueJobsOpt.affinityCount++;
        }
        else
            queueJobsOpt.jobs.push_back(job);
    }
    else if (job->hasFlag(JOB_IS_IO))
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

    wakeUpThreadsNoLock();
}

void ThreadManager::wakeUpThreads()
{
    if (g_ThreadMgr.numWorkers != 1)
    {
        ScopedLock lk(mutexAdd);
        wakeUpThreadsNoLock();
    }
}

void ThreadManager::wakeUpThreadsNoLock()
{
    if (g_ThreadMgr.numWorkers != 1 && !availableThreads.empty())
    {
        const auto thread = availableThreads.back();
        availableThreads.pop_back();
        thread->notifyJob();
    }
}

void ThreadManager::jobHasEnded(Job* job, JobResult result)
{
    ScopedLock lk(mutexAdd);
    ScopedLock lk1(mutexDone);

    SWAG_ASSERT(job->hasFlag(JOB_IS_IN_THREAD));
    job->removeFlag(JOB_IS_IN_THREAD);
    if (job->hasFlag(JOB_IS_OPT))
    {
        SWAG_ASSERT(jobsOptInThreads);
        --jobsOptInThreads;
    }
    else
    {
        SWAG_ASSERT(jobsInThreads);
        --jobsInThreads;
    }

    job->wakeUpBy = nullptr;

    // Some jobs have been registered to be pushed
    for (const auto toRun : job->jobsToAdd)
    {
        toRun->wakeUpBy = job;
        addJobNoLock(toRun);
    }

    job->jobsToAdd.clear();

    // A request has been made to run the job, and the job was not ended
    // So we rerun it immediately
    if (job->hasFlag(JOB_IS_PENDING_RUN))
    {
        job->removeFlag(JOB_IS_PENDING_RUN);
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
        for (const auto toRun : job->dependentJobs.list)
        {
            toRun->wakeUpBy = job;
            addJobNoLock(toRun);
        }

        job->dependentJobs.clear();
    }

    // If I am an IO job, then decrease counter
    if (job->hasFlag(JOB_IS_IO))
    {
        SWAG_ASSERT(currentJobsIO);
        --currentJobsIO;
    }

    // Do we need to wake up my parent job ?
    bool wakeUpParent = false;

    // Only if the job is done
    if (result != JobResult::KeepJobAlive)
        wakeUpParent = true;

    // or if we are waiting for a placeholder symbol to be solved, because the parent job *will* generate the placeholder
    else if (job->waitingSymbolSolved && job->waitingSymbolSolved->is(SymbolKind::PlaceHolder))
        wakeUpParent = true;

    // or if the symbol is waiting for code generation
    else if (job->waitingSymbolSolved && job->waitingSymbolSolved->hasFlag(SYMBOL_ATTRIBUTE_GEN))
        wakeUpParent = true;

    // Notify my parent job that I am done
    if (wakeUpParent && job->dependentJob)
    {
        SWAG_ASSERT(job->dependentJob->waitOnJobs);
        job->dependentJob->waitOnJobs--;
        job->removeFlag(JOB_IS_PENDING);
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
        job->waitingJobIndex = waitingJobs.size() - 1;
    }
    else if (job->jobGroup)
    {
        job->jobGroup->doneJob();
    }

    // Is this the last job ?
    if (doneWithJobs())
        condVarDone.notify_all();
}

namespace
{
#if !defined(SWAG_DEV_MODE)
    void exceptionMessage(Job* job, SWAG_LPEXCEPTION_POINTERS args)
    {
        g_Log.lock();
        g_Log.setColor(LogColor::Red);
        g_Log.print("fatal error: hardware exception during job execution !\n");

        g_Log.setColor(LogColor::White);
        g_Log.messageHeaderDot("exception code", form("0x%08X\n", args->ExceptionRecord->ExceptionCode), LogColor::White, LogColor::White, " ");
        if (job->baseContext && job->baseContext->sourceFile)
        {
            g_Log.printHeaderDot("current source file", job->baseContext->sourceFile->name, LogColor::White, LogColor::White, " ");
            g_Log.printHeaderDot("current module", job->baseContext->sourceFile->module->name, LogColor::White, LogColor::White, " ");
        }

        if (!job->nodes.empty())
        {
            const auto node = job->nodes.back();
            g_Log.printHeaderDot("current node", node->token.text, LogColor::White, LogColor::White, " ");
            if (node->token.sourceFile)
                g_Log.printHeaderDot("current location", form("%s:%d:%d", node->token.sourceFile->path.c_str(), node->token.startLocation.line + 1, node->token.startLocation.column + 1), LogColor::White, LogColor::White, " ");
        }

        g_Log.setDefaultColor();
        g_Log.unlock();
    }

    int exceptionHandler(Job* job, SWAG_LPEXCEPTION_POINTERS args)
    {
        if (OS::isDebuggerAttached() || g_CommandLine.dbgDevMode)
        {
            OS::errorBox("[Developer Mode]", "Hardware exception raised !");
            return EXCEPTION_CONTINUE_EXECUTION;
        }

        exceptionMessage(job, args);
        return SWAG_EXCEPTION_EXECUTE_HANDLER;
    }

#else
    int exceptionHandler(Job* /*job*/, SWAG_LPEXCEPTION_POINTERS /*args*/)
    {
        OS::errorBox("[Developer Mode]", "Hardware exception raised !");
        return EXCEPTION_CONTINUE_EXECUTION;
    }
#endif
}

void ThreadManager::executeOneJob(Job* job)
{
    SWAG_TRY
    {
        const auto result = job->execute();
        jobHasEnded(job, result);
        if (result == JobResult::ReleaseJob)
            job->release();
    }
    SWAG_EXCEPT(exceptionHandler(job, SWAG_GET_EXCEPTION_INFOS()))
    {
        OS::exit(-1);
    }
}

bool ThreadManager::doneWithJobs() const
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
        for (const auto p : queueJobsOpt.jobs)
            p->release();
        queueJobsOpt.jobs.clear();

        for (uint32_t i = 0; i < numWorkers; i++)
        {
            for (const auto p : queueJobsOpt.affinity[i])
                p->release();
            queueJobsOpt.affinity[i].clear();
        }
        queueJobsOpt.affinityCount = 0;
    }

    while (jobsOptInThreads)
        std::this_thread::yield();
}

void ThreadManager::waitEndJobsSync()
{
    while (tryExecuteJob())
    {
    }
}

void ThreadManager::waitEndJobs()
{
    // If one core only, do the jobs right now, in order
    if (g_ThreadMgr.numWorkers == 1)
    {
        waitEndJobsSync();
        return;
    }

    // Else wait for all jobs to be done in other threads
    while (true)
    {
        std::unique_lock lk(mutexDone);
        if (doneWithJobs())
            break;
        condVarDone.wait(lk);
    }
}

void ThreadManager::eatJob(Job* job)
{
    SWAG_ASSERT(!job->hasFlag(JOB_IS_IN_THREAD));
    job->addFlag(JOB_IS_IN_THREAD);

    job->setPendingInfos(JobWaitKind::None);
    job->jobThread = nullptr;

    if (job->hasFlag(JOB_IS_OPT))
        ++jobsOptInThreads;
    else
        ++jobsInThreads;
}

Job* ThreadManager::getJob(JobQueue& queue)
{
    {
        SharedLock lk0(mutexAdd);
        if (queue.jobs.empty() && !queue.affinityCount)
            return nullptr;
    }

    ScopedLock lk0(mutexAdd);
    const bool jEmpty = queue.jobs.empty();
    if (jEmpty && !queue.affinityCount)
        return nullptr;

    ScopedLock lk1(mutexDone);
    Job*       job;

    if (!queue.affinity[g_ThreadIndex].empty())
    {
        job = queue.affinity[g_ThreadIndex].back();
        if (debuggerMode && !job->hasFlag(JOB_IS_DEBUGGER))
            return nullptr;
        queue.affinity[g_ThreadIndex].pop_back();
        SWAG_ASSERT(queue.affinityCount);
        queue.affinityCount--;
    }
    else if (jEmpty)
    {
        job = nullptr;
        for (uint32_t i = 0; i < queue.affinity.size(); i++)
        {
            if (!queue.affinity[i].empty())
            {
                job = queue.affinity[i].back();
                if (debuggerMode && !job->hasFlag(JOB_IS_DEBUGGER))
                    return nullptr;
                queue.affinity[i].pop_back();
                SWAG_ASSERT(queue.affinityCount);
                queue.affinityCount--;
                break;
            }
        }
    }
    else
    {
        auto jobPickIndex = queue.jobs.size() - 1;
#ifdef SWAG_DEV_MODE
        if (g_CommandLine.randomize && !debuggerMode)
            jobPickIndex = static_cast<uint32_t>(rand()) % queue.jobs.count;
#endif
        job = queue.jobs[jobPickIndex];
        if (debuggerMode && !job->hasFlag(JOB_IS_DEBUGGER))
            return nullptr;
        queue.jobs.erase(jobPickIndex);
    }

    SWAG_ASSERT(job);
    SWAG_ASSERT(job->hasFlag(JOB_IS_IN_QUEUE));
    job->removeFlag(JOB_IS_IN_QUEUE);
    eatJob(job);

    return job;
}

Job* ThreadManager::getJob()
{
    Job* job;

    // If no IO is running, then we can take one in priority
    if (!currentJobsIO.load())
    {
        job = getJob(queueJobsIO);
        if (job)
        {
            ++currentJobsIO;
            return job;
        }
    }

    // Else we take a normal job if we can
    job = getJob(queueJobs);
    if (job)
        return job;

    // Else we take an optional job if we can
    job = getJob(queueJobsOpt);
    if (job)
        return job;

    // Otherwise, then IO, as there's nothing left
    job = getJob(queueJobsIO);
    if (job)
    {
        ++currentJobsIO;
        return job;
    }

    // Otherwise, steal a syntax job from a module if we can
    SharedLock lk(g_Workspace->mutexModules);
    for (const auto p : g_Workspace->modules)
    {
        job = p->syntaxJobGroup.pickJob();
        if (job)
        {
            SWAG_ASSERT(job->hasFlag(JOB_IS_OPT));
            eatJob(job);
            return job;
        }
    }

    return nullptr;
}

Job* ThreadManager::getJob(JobThread* thread)
{
    if (const auto job = getJob())
    {
        job->jobThread = thread;
        return job;
    }

    std::unique_lock lk(thread->mutexNotify);
    mutexAdd.lock();
    availableThreads.push_back(thread);
    mutexAdd.unlock();
    thread->condVar.wait(lk);

    return nullptr;
}

bool ThreadManager::tryExecuteJob()
{
    const auto job = getJob();
    if (!job)
        return false;
    executeOneJob(job);
    return true;
}
