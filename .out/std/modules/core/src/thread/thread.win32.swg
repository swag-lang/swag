#global if WINDOWS
#global public
#global namespace thread
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

    th.context.tempAllocator.block = null
    th.context.tempAllocator.used = 0
    @setcontext(th.context)

    th.userLambda(:th)
}

// Wait multiple threads
func wait(threads: *Thread...)
{
    var handles: [MAXIMUM_WAIT_OBJECTS] HANDLE

    total := @countof(threads)
    while total > 0
    {
        toTreat := math.min(MAXIMUM_WAIT_OBJECTS, cast(u32) total)
        loop i: toTreat
        {
            debug.assert(threads[i].isValid())
            handles[i] = threads[i].handle
        }

        assume WaitForMultipleObjects(toTreat, &handles[0], TRUE, INFINITE)
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
    // Initialize a thread in pause state
    func init(using self, lambda: func(Thread), userParam: *void = null, priority = ThreadPriority.Normal) throw
    {
        debug.assert(!isValid(), "thread already created")
        debug.assert(lambda != null, "lambda parameter is null")

        context = :@getcontext()

        handle = try CreateThread(null, 0, @mkcallback(&threadEntry), self, CREATE_SUSPENDED, &id)
        userLambda = lambda

        self.userParam = #scopefct.userParam
        try setPriority(#scopefct.priority)
    }

    // Resume the given thread, if it was paused
    func start(using self) throw
    {
        debug.assert(isValid())
        try ResumeThread(handle)
    }

    // Set the thread priority
    func setPriority(using self, priority: ThreadPriority) throw
    {
        debug.assert(isValid())

        newPrio := #scopefct.priority
        if self.priority == newPrio
            return
        self.priority = newPrio

        wprio := 0
        #[swag.complete]
        switch newPrio
        {
            case Lowest:        wprio = THREAD_PRIORITY_LOWEST
            case BelowNormal:   wprio = THREAD_PRIORITY_BELOW_NORMAL
            case Normal:        wprio = THREAD_PRIORITY_NORMAL
            case AboveNormal:   wprio = THREAD_PRIORITY_ABOVE_NORMAL
            case Highest:       wprio = THREAD_PRIORITY_HIGHEST
        }

        try SetThreadPriority(handle, THREAD_PRIORITY_LOWEST)
    }

    // Wait for the given thread to be done, and close it
    // After that call, isValid() will return false
    func wait(using self)
    {
        debug.assert(isValid())
        assume WaitForSingleObject(handle, INFINITE)
        CloseHandle(handle)
        handle = null
    }

    // Returns true if the thread has finished
    func isDone(using self)->bool
    {
        debug.assert(isValid())
        res := assume WaitForSingleObject(handle, 0)
        return res != WAIT_TIMEOUT
    }
}