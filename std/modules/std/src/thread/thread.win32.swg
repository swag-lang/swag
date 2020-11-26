#if #os == "windows";
public namespace std.thread;
using win32

// Sleep the current thread for a given amount of milliseconds
func sleep(ms: u32)
{
    Sleep(ms)
}

// Sleep the current thread for a given amount of milliseconds
func yield()
{
    SwitchToThread()
}

#[swag.callback]
private func threadEntry(param: *void)
{
    th := cast(*Thread) param
    @setcontext(th.context)

    th.userLambda(:th)
}

// Creates a new thread
func create(th: *Thread, lambda: func(Thread), userParam: *void = null, priority = ThreadPriority.Normal) -> ErrorId
{
    debug.assert(!th.isValid(), "thread already created")
    debug.assert(lambda != null, "lambda parameter is null")
    th.context = :@getcontext()

    // As the threadEntry will be called by the os, need to be sure
    // we have a valid function pointer whatever the case is :
    // - we are running bytecode, and threadEntry should be run by the compiler
    // - we are native, and threadEntry is like a foreign function (native, i.e. callback)
    var l64 = cast(u64) &threadEntry
    if l64 & swag.LambdaBcMarker
        l64 = autocast @thrunptr()
    else
    {
        th.context.flags &= ~swag.ContextFlags.ByteCode
        l64 &= ~swag.LambdaForeignMarker
    }

    h := CreateThread(null, 0, cast(*void) l64, th, CREATE_SUSPENDED, &th.id)
    if h == null
    {
        error.setLastErrorId()
        return error.last()
    }

    th.userLambda = lambda
    th.userParam = userParam
    th.handle = h
    th.setPriority(priority)

    return null
}

// Wait multiple threads
func wait(threads: *Thread...)
{
    const BATCH_COUNT = 64'u32
    var handles: [BATCH_COUNT] HANDLE

    total := @countof(threads)
    while total > 0
    {
        toTreat := math.min(BATCH_COUNT, total)
        loop i: toTreat
        {
            //debug.assert(threads[i].isValid())
            //handles[i] = threads[i].handle
        }

        WaitForMultipleObjects(toTreat, &handles[0], TRUE, INFINITE)
        total -= toTreat
    }

    // Close all
    visit th: threads
    {
        CloseHandle(th.handle)
        th.handle = null
    }
}

impl Thread
{
    // Resume the given thread, if it was paused
    func start(using self)->ErrorId
    {
        debug.assert(isValid())
        if ResumeThread(handle) == cast(DWORD) -1
        {
            error.setLastErrorId()
            return error.last()
        }

        return null
    }

    // Set the thread priority
    func setPriority(using self, newPriority: ThreadPriority)
    {
        debug.assert(isValid())

        if priority == newPriority
            return
        priority = newPriority

        using ThreadPriority
        #[swag.complete]
        switch priority
        {
            case Lowest:        SetThreadPriority(handle, THREAD_PRIORITY_LOWEST)
            case BelowNormal:   SetThreadPriority(handle, THREAD_PRIORITY_BELOW_NORMAL)
            case Normal:        SetThreadPriority(handle, THREAD_PRIORITY_NORMAL)
            case AboveNormal:   SetThreadPriority(handle, THREAD_PRIORITY_ABOVE_NORMAL)
            case Highest:       SetThreadPriority(handle, THREAD_PRIORITY_HIGHEST)
        }
    }

    // Wait for the given thread to be done, and close it
    // After that call, isValid() will return false
    func wait(using self)
    {
        debug.assert(isValid())
        WaitForSingleObject(handle, INFINITE)
        CloseHandle(handle)
        handle = null
    }

    // Returns true if the thread has finished
    func isDone(using self)->bool
    {
        debug.assert(isValid())
        return WaitForSingleObject(handle, 0) != WAIT_TIMEOUT
    }
}