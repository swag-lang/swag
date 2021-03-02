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
public func isSynchrone() => g_numWorkers == 0

// Returns number of threads in the job system
public func getNumWorkers() => g_numWorkers

// Set the number of worker threads. Must be done once
public func setNumWorkers(numWorkers: u32 = 0) throw
{
    debug.assert(g_numWorkers == 0)
    g_numWorkers = numWorkers ?? env.getProcessorCount()
    g_workers.resize(cast(uint) g_numWorkers)
    loop i: g_numWorkers
        try g_workers[i].init(&workerEntry)
    visit *th: g_workers
        try th.start()
}

// Schedule a job to execute
public func scheduleJob(job: *Job)
{
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

#init
{
    try g_mutexPendingJobs.init()
    try g_eventPendingJobs.init()
    try g_eventDone.init(true, true)
}