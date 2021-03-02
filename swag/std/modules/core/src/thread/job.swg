#global namespace jobsystem
using thread, swag

#[strict]
public alias JobHandle = *void

struct Job
{
    lambda:         func(*void)
    data:           *void
    createFlags:    JobCreateFlags = ?
}

var g_Allocator:            IAllocator
var g_created:              bool
var g_numWorkers:           u32
var g_workers:              Array'Thread
var g_pendingJobs:          Array'(*Job)
var g_freeJobs:             Array'(*Job)
var g_mutexPendingJobs:     sync.Mutex
var g_mutexFreeJobs:        sync.Mutex
var g_eventPendingJobs:     sync.Event
var g_eventDone:            sync.Event
var g_totalJobs:            u32

func workerEntry(th: Thread)
{
    while true
    {
        while peekExecJob() {}
        g_eventPendingJobs.wait()
    }
}

func peekExecJob()->bool
{
    job := getJobToExec()
    if !job return false

    // Execute job
    job.lambda(job.data)

    // We are done with that job
    if job.createFlags & JobCreateFlags.AutoRelease
        releaseJob(job)

    // Done with all jobs ?
    sync.scopedLock(g_mutexPendingJobs)
    debug.assert(g_totalJobs != 0)
    g_totalJobs -= 1
    if g_totalJobs == 0
        g_eventDone.signal()

    return true
}

func getJobToExec()->*Job
{
    sync.scopedLock(g_mutexPendingJobs)
    if g_pendingJobs.isEmpty()
        return null
    return g_pendingJobs.popBack()
}

#[swag.enumflags]
public enum JobCreateFlags : u32
{
    Zero        = 0x00000000
    AutoRelease = 0x00000001
}

// Returns true is the job system has been initialized
public func isValid() => g_created

// Returns number of threads in the job system
public func getNumWorkers() => g_numWorkers

// Setup the job system
public func init(numCores: u32 = 0) throw
{
    debug.assert(!g_created)
    g_created = true

    g_Allocator = @getcontext().allocator
    try g_mutexPendingJobs.init()
    try g_mutexFreeJobs.init()
    try g_eventPendingJobs.init()
    try g_eventDone.init(true, true)

    g_workers.allocator = g_Allocator
    g_pendingJobs.allocator = g_Allocator
    g_freeJobs.allocator = g_Allocator

    // If numCores is 1, then jobsystem will by synchrone
    g_numWorkers = numCores ?? env.getProcessorCount()
    if g_numWorkers != 1
    {
        g_workers.resize(cast(uint) g_numWorkers)
        loop i: g_numWorkers
            try g_workers[i].init(&workerEntry)
        visit *th: g_workers
            try th.start()
    }
}

// Creates a new job, but does not run it
public func createJob(lambda: func(*void), jobData: *void = null, flags = JobCreateFlags.Zero)->JobHandle
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
    result.createFlags = flags
    return result
}

// Release a job
public func releaseJob(job: JobHandle)
{
    debug.assert(g_created, "jobsystem is not initialized")
    sync.scopedLock(g_mutexFreeJobs)
    g_freeJobs.add(job)
}

// Schedule a job to execute
public func scheduleJob(job: JobHandle)
{
    debug.assert(g_created, "jobsystem is not initialized")
    sync.scopedLock(g_mutexPendingJobs)
    g_totalJobs += 1
    g_eventDone.reset()
    g_pendingJobs.add(job)
    g_eventPendingJobs.signal()
}

// Wait for all jobs to be finished
public func waitDone()
{
    while peekExecJob() {}
    g_eventDone.wait()
}