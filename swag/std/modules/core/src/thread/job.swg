#global namespace thread
using thread

public struct Job
{
    data:   *void
    lambda: func(*JobSystem, *void)
}

public struct JobSystem
{
    created:            bool
    workers:            Array'Thread
    pendingJobs:        Array'(*Job)
    freeJobs:           Array'(*Job)
    mutexPendingJobs:   sync.Mutex
    mutexFreeJobs:      sync.Mutex
}

private func workerEntry(th: Thread)
{
    js := cast(*JobSystem) th.userParam
}

public impl JobSystem
{
    // Get the job system global instance
    func get() => &g_JobSystem

    // Setup the job system
    func create(using self, numCores: u32 = 0) throw
    {
        debug.assert(!created)
        created = true

        try mutexPendingJobs.init()
        try mutexFreeJobs.init()

        num := numCores ?? env.getProcessorCount()
        loop num
        {
            var th: Thread
            try th.init(&workerEntry, self)
            g_JobSystem.workers.emplace(&th)
        }

        visit *th: workers
        {
            try th.start()
        }
    }

    func newJob(using self)->*Job
    {
        sync.scopedLock(mutexFreeJobs)
        if freeJobs.isEmpty()
            return memory.new'Job()
        return null//freeJobs.popBack()
    }
}

var g_JobSystem: JobSystem