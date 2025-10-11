#global #if WINDOWS
#global public
#global namespace Threading
using Win32

internal func threadEntry(param: *void)->DWORD
{
    let th = cast(*Thread) param
    @setcontext(th.context)
    th.userLambda(dref th)
    th.context.tempAllocator.release()
    return 0
}

// Wait multiple threads
func wait(threads: *Thread...)
{
    var handles: [MAXIMUM_WAIT_OBJECTS] HANDLE

    var total = @countof(threads)
    while total > 0
    {
        let toTreat = Math.min(MAXIMUM_WAIT_OBJECTS, cast(u32) total)
        for i in toTreat
        {
            Debug.assert(threads[i].isValid())
            handles[i] = threads[i].handle
        }

        assume WaitForMultipleObjects(toTreat, &handles[0], TRUE, INFINITE)
        total -= toTreat
    }

    // Close all
    foreach th in threads
    {
        CloseHandle(th.handle)
        th.handle = null
    }
}

impl Thread
{
    // Sleep the current thread for a given amount of milliseconds
    func sleep(ms: Time.Duration)
    {
        Sleep(ms.toMs())
    }

    // Sleep the current thread for a given amount of milliseconds
    func yield()
    {
        SwitchToThread()
    }

    // Initialize a thread in pause state
    mtd init(lambda: func||(Thread), userParam: *void = null, priority = ThreadPriority.Normal, tempAllocSize: s32 = -1) throw
    {
        Debug.assert(!.isValid(), "thread already created")

        // Get a copy of current context, and initialize a temporary allocator for that thread
        .context = dref @getcontext()
        @init(&.context.tempAllocator, 1)
        if tempAllocSize < 0 do
            .context.tempAllocator.setCapacity(.context.tempAllocator.capacity, .context.allocator)
        elif tempAllocSize != 0 do
            .context.tempAllocator.setCapacity(cast(u64) tempAllocSize, .context.allocator)

        .handle     = try CreateThread(null, 0, @mkcallback(&threadEntry), me, CREATE_SUSPENDED, &.id)
        .userLambda = lambda
        .requestEnd = false

        .userParam = userParam
        try .setPriority(priority)
    }

    // Resume the given thread, if it was paused
    mtd start() throw
    {
        Debug.assert(.isValid())
        try ResumeThread(.handle)
    }

    // Set the thread priority
    mtd setPriority(priority: ThreadPriority) throw
    {
        Debug.assert(.isValid())

        let newPrio = priority
        if .priority == newPrio do
            return
        .priority = newPrio

        var wprio = 0
        #[Swag.Complete]
        switch newPrio
        {
        case Lowest:
            wprio = THREAD_PRIORITY_LOWEST
        case BelowNormal:
            wprio = THREAD_PRIORITY_BELOW_NORMAL
        case Normal:
            wprio = THREAD_PRIORITY_NORMAL
        case AboveNormal:
            wprio = THREAD_PRIORITY_ABOVE_NORMAL
        case Highest:
            wprio = THREAD_PRIORITY_HIGHEST
        }

        try SetThreadPriority(.handle, THREAD_PRIORITY_LOWEST)
    }

    // Wait for the given thread to be done, and close it
    // After that call, isValid() will return false
    mtd wait()
    {
        Debug.assert(.isValid())
        assume WaitForSingleObject(.handle, INFINITE)
        CloseHandle(.handle)
        .handle = null
    }

    // Returns true if the thread has finished
    mtd isDone()->bool
    {
        Debug.assert(.isValid())
        let res = assume WaitForSingleObject(.handle, 0)
        return res != WAIT_TIMEOUT
    }
}
