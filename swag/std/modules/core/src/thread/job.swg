#global namespace thread
using thread

public struct Job
{
    data:   *void
    lambda: func(*void, *void)
}

public struct JobSystem
{
    created:        bool
    workers:        Array'Thread
    pendingJobs:    Array'Job
    mutexJobs:      sync.Mutex
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

        try mutexJobs.init()

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
}

var g_JobSystem: JobSystem