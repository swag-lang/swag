#global marked
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
        Debug.assert(!me.isValid(), "thread already created")

        // Get a copy of current context, and initialize a temporary allocator for that thread
        me.context = dref @getcontext()
        @init(&me.context.tempAllocator, 1)
        if tempAllocSize < 0:
            me.context.tempAllocator.setCapacity(me.context.tempAllocator.capacity, me.context.allocator)
        elif tempAllocSize != 0:
            me.context.tempAllocator.setCapacity(cast(u64) tempAllocSize, me.context.allocator)

        me.handle     = try CreateThread(null, 0, @mkcallback(&threadEntry), me, CREATE_SUSPENDED, &me.id)
        me.userLambda = lambda
        me.requestEnd = false

        me.userParam = #me.userParam
        try me.setPriority(#me.priority)
    }

    // Resume the given thread, if it was paused
    mtd start() throw
    {
        Debug.assert(me.isValid())
        try ResumeThread(me.handle)
    }

    // Set the thread priority
    mtd setPriority(priority: ThreadPriority) throw
    {
        Debug.assert(me.isValid())

        let newPrio = #me.priority
        if me.priority == newPrio:
            return
        me.priority = newPrio

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

        try SetThreadPriority(me.handle, THREAD_PRIORITY_LOWEST)
    }

    // Wait for the given thread to be done, and close it
    // After that call, isValid() will return false
    mtd wait()
    {
        Debug.assert(me.isValid())
        assume WaitForSingleObject(me.handle, INFINITE)
        CloseHandle(me.handle)
        me.handle = null
    }

    // Returns true if the thread has finished
    mtd isDone()->bool
    {
        Debug.assert(me.isValid())
        let res = assume WaitForSingleObject(me.handle, 0)
        return res != WAIT_TIMEOUT
    }
}
