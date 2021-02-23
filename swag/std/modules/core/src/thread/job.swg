#global namespace thread
using thread

public struct JobSystem
{
    created:    bool
    active:     bool
    workers:    Array'Thread
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

        num := numCores ?? env.getProcessorCount()
        loop num
        {
            var th: Thread
            try th.init(&workerEntry, self)
            g_JobSystem.workers.emplace(&th)
        }
    }

    // Run the job system
    func run(using self) throw
    {
        debug.assert(!active)
        active = true

        visit *th: workers
        {
            try th.start()
        }
    }
}

var g_JobSystem: JobSystem