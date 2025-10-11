#global namespace Jobs
using Threading, Swag

#[Swag.EnumFlags]
public enum JobState
{
    Zero      = 0x00000000
    InPending = 0x00000001
    Done      = 0x00000002
}

public struct Job
{
    lambda:     func(*void)
    data:       *void

    // Internal state
    state:            JobState = Zero
    pendingIndex:     u32 = Swag.U32.Max
}

public struct(T) SliceJob
{
    using base:     Job
    buffer:         [..] T
    offset:         u32
    userData:       *void
}

public struct ForJob
{
    using base:     Job
    startIndex:     u32
    endIndex:       u32
    userData:       *void
}

var g_Allocator:        Swag.IAllocator
var g_mustEnd:          bool
var g_numWorkers:       s32
var g_workers:          Array'Thread
var g_pendingJobs:      Array'(*Job)
var g_mutexPendingJobs: Sync.Mutex
var g_eventPendingJobs: Sync.Event
var g_eventDone:        Sync.Event
var g_totalJobs:        u32

#drop
{
    if g_numWorkers
    {
        g_mustEnd = true
        g_eventPendingJobs.signal()
        while Atomic.get(&g_numWorkers)
        {
        }

        foreach th in g_workers do
            th.wait()
        g_workers.free()
        g_pendingJobs.free()
    }
}

func workerEntry(_th: Thread)
{
    while !g_mustEnd
    {
        while peekExecJob()
        {
        }
        g_eventPendingJobs.wait()
    }

    Atomic.add(&g_numWorkers, -1)
    g_eventPendingJobs.signal()
}

func runJob(job: *Job)
{
    // Execute job
    job.lambda(job.data)

    // Done
    Sync.scopedLock(&g_mutexPendingJobs)
    setState(job, JobState.Done)
    Debug.assert(g_totalJobs != 0)
    g_totalJobs -= 1
    if g_totalJobs == 0 do
        g_eventDone.signal()
}

func peekExecJob()->bool
{
    let job = getJobToExec()
    if !job do
        return false
    runJob(job)
    return true
}

func getJobToExec()->#null *Job
{
    Sync.scopedLock(&g_mutexPendingJobs)
    if g_pendingJobs.isEmpty() do
        return null
    let job = g_pendingJobs.popBack()
    Debug.assert(job.testState(JobState.InPending))
    job.clearState(JobState.InPending)
    return job
}

func removeJobNoLock(job: *Job)
{
    g_pendingJobs.removeAt(cast(u64) job.pendingIndex)
    if job.pendingIndex < g_pendingJobs.count do
        g_pendingJobs[job.pendingIndex].pendingIndex = job.pendingIndex
    job.pendingIndex = Swag.U32.Max
    job.clearState(JobState.InPending)
}

#[Swag.Discardable]
func clearState(job: *Job, state: JobState) => Atomic.logicAnd(cast(*u32) &job.state, cast(u32) ~state)
#[Swag.Discardable]
func setState(job: *Job, state: JobState) => Atomic.logicOr(cast(*u32) &job.state, cast(u32) state)
func testState(job: *Job, state: JobState) => Atomic.get(cast(*u32) &job.state) & cast(u32) state != 0

// Returns true is the job system has been initialized
public func isSynchrone()->bool => g_numWorkers == 0

// Returns number of threads in the job system
public func getNumWorkers()->u32 => cast(u32) g_numWorkers

// Set the number of worker threads. Must be done once
public func setNumWorkers(numWorkers: u32 = 0) throw
{
    Debug.assert(g_numWorkers == 0)

    g_Allocator             = @getcontext().allocator
    g_workers.allocator     = g_Allocator
    g_pendingJobs.allocator = g_Allocator

    g_eventPendingJobs.init()
    g_eventDone.init(true, true)

    g_numWorkers = numWorkers orelse Hardware.getProcessorCount()
    g_workers.resize(cast(u64) g_numWorkers)
    foreach &th in g_workers do
        try th.init(&workerEntry)
    foreach &th in g_workers do
        try th.start()
}

// Schedule a job to execute
public func scheduleJob(job: *Job)
{
    Sync.scopedLock(&g_mutexPendingJobs)
    job.setState(JobState.InPending)
    g_totalJobs += 1
    g_eventDone.reset()
    job.pendingIndex = cast(u32) g_pendingJobs.count
    g_pendingJobs.add(job)
    g_eventPendingJobs.signal()
}

// Wait for all registered jobs to be finished
public func waitDone()
{
    while peekExecJob()
    {
    }
    g_eventDone.wait()
}

// Wait for a given job to be finished
public func waitJob(job: *Job)
{
    // Fast exit, without lock
    if job.testState(JobState.Done) do
        return

    g_mutexPendingJobs.lock()

    // Job already done
    if job.testState(JobState.Done)
    {
        g_mutexPendingJobs.unlock()
        return
    }

    // Job still in pending list, we can execute it right away
    // and leave
    if job.testState(JobState.InPending)
    {
        removeJobNoLock(job)
        g_mutexPendingJobs.unlock()
        runJob(job)
        return
    }

    // Here, job is being processed by another thread, so we participate
    // to job execution until done flag is set
    g_mutexPendingJobs.unlock()
    while !job.testState(JobState.Done)
    {
        discard peekExecJob()
    }
}

// Operate on a range in parallel chunks.
// Exposed variables:
// - buffer: address of the element of the range to process
// - data:   userData as passed to the macro
#[Swag.Macro]
public func(T) parallelVisit(range: [..] T, offset: u32 = 1, userData: *void = null, stmt: #code void)
{
    if !@countof(range) do
        return
    Debug.assert(offset != 0)

    func jobEntry(jobData: *void)
    {
        let oneJob = cast(*SliceJob'T) jobData
        visitSlice(oneJob.buffer, oneJob.offset, oneJob.userData)
    }

    func visitSlice(buffer: [..] T, offset: u32, userData: #null *void)
    {
        var scan    = @dataof(buffer)
        let countof = @countof(buffer)
        for countof / offset
        {
            #macro
            {
                let #alias0 = #up scan
                let #alias1 = #up userData
                #inject(#up stmt)
            }

            scan += offset
        }
    }

    // Force synchrone execution if range is too small
    let wh         = @countof(range) / offset
    let numWorkers = Math.min(getNumWorkers(), cast(u32) wh)
    if isSynchrone() or numWorkers <= 1
    {
        visitSlice(range, offset, userData)
        return
    }

    // Compute size of each chunk (and size of the last one)
    var splitSize     = wh / numWorkers
    var lastSplitSize = splitSize + (wh - (splitSize * numWorkers))
    splitSize *= offset
    lastSplitSize *= offset

    // Launch jobs
    var sliceJobs: Array'(SliceJob'T)
    sliceJobs.resize(cast(u64) numWorkers)

    var ptr = @dataof(range)
    foreach &j in sliceJobs
    {
        let sizeSlice = @index == sliceJobs.count - 1 ? lastSplitSize : splitSize

        j.base.lambda = &jobEntry
        j.base.data   = j
        j.offset      = offset
        j.userData    = userData
        j.buffer      = @mkslice(ptr, sizeSlice)
        ptr += sizeSlice

        scheduleJob(j)
    }

    // Sync
    foreach &j in sliceJobs do
        waitJob(j)
}

// Do a for for in parallel chunks.
// Exposed variables:
// - #alias0: current for index
// - #alias1: userData as passed to the macro
#[Swag.Macro]
public func parallelFor(count: u32, userData: *void = null, stmt: #code void)
{
    if !count do
        return

    func jobEntry(jobData: *void)
    {
        let oneJob = cast(*ForJob) jobData
        visitFor(oneJob.startIndex, oneJob.endIndex, oneJob.userData)
    }

    func visitFor(startIndex, endIndex: u32, userData: #null *void)
    {
        var curIndex = startIndex
        for (endIndex - startIndex) + 1
        {
            #macro
            {
                let #alias0 = #up curIndex
                let #alias1 = #up userData
                #inject(#up stmt)
            }

            curIndex += 1
        }
    }

    // Force synchrone execution if range is too small
    let numWorkers = Math.min(getNumWorkers(), count)
    if isSynchrone() or numWorkers <= 1
    {
        visitFor(0, count - 1, userData)
        return
    }

    // Compute size of each chunk (and size of the last one)
    let splitSize     = count / numWorkers
    let lastSplitSize = splitSize + (count - (splitSize * numWorkers))

    // Launch jobs
    var sliceJobs: Array'ForJob
    sliceJobs.resize(cast(u64) numWorkers)

    var startIndex = 0'u32
    foreach &j in sliceJobs
    {
        let sizeSlice = @index == sliceJobs.count - 1 ? lastSplitSize : splitSize

        j.base.lambda = &jobEntry
        j.base.data   = j
        j.startIndex  = startIndex
        j.endIndex    = startIndex + sizeSlice - 1
        j.userData    = userData
        startIndex += sizeSlice

        scheduleJob(j)
    }

    // Sync
    foreach &j in sliceJobs do
        waitJob(j)
}
