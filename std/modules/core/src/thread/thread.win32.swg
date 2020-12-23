#if #os == "windows";
public namespace thread;
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
    // Initialize a thread in pause state
    func init(using self, lambda: func(Thread), userParam: *void = null, priority = ThreadPriority.Normal)->ErrorId
    {
        debug.assert(!isValid(), "thread already created")
        debug.assert(lambda != null, "lambda parameter is null")
        context = :@getcontext()

        var cb = system.lambdaToCallback(&threadEntry)
        h := CreateThread(null, 0, cb, self, CREATE_SUSPENDED, &id)
        if h == null
        {
            error.setLastErrorId()
            return error.last()
        }

        userLambda = lambda
        handle = h
        self.userParam = #scopefct.userParam
        setPriority(#scopefct.priority)
        return null
    }

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
    func setPriority(using self, priority: ThreadPriority)
    {
        debug.assert(isValid())

        newPrio := #scopefct.priority
        if self.priority == newPrio
            return
        self.priority = newPrio

        using ThreadPriority
        #[swag.complete]
        switch newPrio
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