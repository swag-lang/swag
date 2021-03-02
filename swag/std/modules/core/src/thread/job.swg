#global namespace jobsystem
using thread, swag

public struct Job
{
    lambda: func(*void)
    data:   *void
}

var g_created:              bool
var g_numWorkers:           u32
var g_workers:              Array'Thread
var g_pendingJobs:          Array'(*Job)
var g_mutexPendingJobs:     sync.Mutex
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

// Returns true is the job system has been initialized
public func isValid() => g_created

// Returns number of threads in the job system
public func getNumWorkers() => g_numWorkers

// Setup the job system
public func init(numCores: u32 = 0) throw
{
    debug.assert(!g_created)
    g_created = true

    try g_mutexPendingJobs.init()
    try g_eventPendingJobs.init()
    try g_eventDone.init(true, true)

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

// Schedule a job to execute
public func scheduleJob(job: *Job)
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