#global if WINDOWS
#global public
#global namespace Threading
using Win32

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

#[Swag.callback]
private func threadEntry(param: *void)
{
    th := cast(*Thread) param
    @setcontext(th.context)
    th.userLambda(dref th)
    th.context.tempAllocator.release()
}

// Wait multiple threads
func wait(threads: *Thread...)
{
    var handles: [MAXIMUM_WAIT_OBJECTS] HANDLE

    total := @countof(threads)
    while total > 0
    {
        toTreat := Math.min(MAXIMUM_WAIT_OBJECTS, cast(u32) total)
        loop i: toTreat
        {
            Debug.assert(threads[i].isValid())
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
    func init(using self, lambda: func(Thread), userParam: *void = null, priority = ThreadPriority.Normal, tempAllocSize: s32 = -1) throw
    {
        Debug.assert(!isValid(), "thread already created")
        Debug.assert(lambda != null, "lambda parameter is null")

        // Get a copy of current context, and initialize a temporary allocator for that thread
        context = dref @getcontext()
        @init(&context.tempAllocator)
        if tempAllocSize < 0
            context.tempAllocator.setup()
        else if tempAllocSize != 0
            context.tempAllocator.setCapacity(cast(uint) tempAllocSize)

        handle = try CreateThread(null, 0, @mkcallback(&threadEntry), self, CREATE_SUSPENDED, &id)
        userLambda = lambda

        self.userParam = #scopefct.userParam
        try setPriority(#scopefct.priority)
    }

    // Resume the given thread, if it was paused
    func start(using self) throw
    {
        Debug.assert(isValid())
        try ResumeThread(handle)
    }

    // Set the thread priority
    func setPriority(using self, priority: ThreadPriority) throw
    {
        Debug.assert(isValid())

        newPrio := #scopefct.priority
        if self.priority == newPrio
            return
        self.priority = newPrio

        wprio := 0
        #[Swag.complete]
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
        Debug.assert(isValid())
        assume WaitForSingleObject(handle, INFINITE)
        CloseHandle(handle)
        handle = null
    }

    // Returns true if the thread has finished
    func isDone(using self)->bool
    {
        Debug.assert(isValid())
        res := assume WaitForSingleObject(handle, 0)
        return res != WAIT_TIMEOUT
    }
}