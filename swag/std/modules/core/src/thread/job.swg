#global namespace jobsystem
using thread, swag

#[strict]
public alias JobHandle = *void

struct Job
{
    lambda: func(*void)
    data:   *void
}

var g_Allocator:            IAllocator
var g_created:              bool
var g_workers:              Array'Thread
var g_pendingJobs:          Array'(*Job)
var g_freeJobs:             Array'(*Job)
var g_mutexPendingJobs:     sync.Mutex
var g_mutexFreeJobs:        sync.Mutex
var g_eventPendingJobs:     sync.Event

func workerEntry(th: Thread)
{
    while true
    {
        job := getJobToProcess()
        if job
        {
            job.lambda(job.data)
            job.isDone()
            continue
        }

        g_eventPendingJobs.wait()
    }
}

func isDone(job: *Job)
{
}

func getJobToProcess()->*Job
{
    sync.scopedLock(g_mutexPendingJobs)
    if g_pendingJobs.isEmpty()
        return null
    return g_pendingJobs.popBack()
}

// Setup the job system
public func init(numCores: u32 = 0) throw
{
    debug.assert(!g_created)
    g_created = true

    g_Allocator = @getcontext().allocator
    try g_mutexPendingJobs.init()
    try g_mutexFreeJobs.init()
    try g_eventPendingJobs.init()

    g_workers.allocator = g_Allocator
    g_pendingJobs.allocator = g_Allocator
    g_freeJobs.allocator = g_Allocator

    // If numCores is 1, then jobsystem will by synchrone
    num := numCores ?? env.getProcessorCount()
    if num != 1
    {
        g_workers.resize(cast(uint) num)
        loop i: num
            try g_workers[i].init(&workerEntry)
        visit *th: g_workers
            try th.start()
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
    result.lambda = lambda
    result.data = jobData
    return result
}

// Schedule a job to execute
public func scheduleJob(job: JobHandle)
{
    debug.assert(g_created, "jobsystem is not initialized")
    sync.scopedLock(g_mutexPendingJobs)
    g_pendingJobs.add(job)
    g_eventPendingJobs.signal()
}

// Schedule for all jobs to be done
public func wait()
{
}