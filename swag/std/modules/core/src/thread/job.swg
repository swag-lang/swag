#global namespace jobsystem
using thread, swag

#[strict]
public alias JobHandle = *void

struct Job
{
    data:   *void
    lambda: func(*void)
}

var g_Allocator:            IAllocator
var g_created:              bool
var g_workers:              Array'Thread
var g_pendingJobs:          Array'(*Job)
var g_freeJobs:             Array'(*Job)
var g_mutexPendingJobs:     sync.Mutex
var g_mutexFreeJobs:        sync.Mutex

func workerEntry(th: Thread)
{
}

// Setup the job system
public func init(numCores: u32 = 0) throw
{
    debug.assert(!g_created)
    g_created = true

    g_Allocator = @getcontext().allocator
    try g_mutexPendingJobs.init()
    try g_mutexFreeJobs.init()

    // If numCores is 1, then jobsystem will by synchrone
    num := numCores ?? env.getProcessorCount()
    if num != 1
    {
        loop num
        {
            var th: Thread
            try th.init(&workerEntry)
            g_workers.emplace(&th)
        }

        visit *th: g_workers
        {
            try th.start()
        }
    }
}

// Creates a new job, but does not run it
public func createJob(lambda: func(*void), jobData: *void = null)->JobHandle
{
    debug.assert(g_created, "jobsystem is not initialized")
    sync.scopedLock(g_mutexFreeJobs)

    var result: *Job = ?
    if g_freeJobs.isEmpty()
        result = memory.new'Job(g_Allocator)
    else
        result = g_freeJobs.popBack()
    result.data = jobData
    result.lambda = lambda
    return result
}
