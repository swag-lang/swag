#if #os == "windows";
public namespace std.thread;
using win32

// Sleep the current thread for a given amount of milliseconds
func sleep(ms: u32)
{
    Sleep(ms)
}

#[swag.callback]
private func threadEntry(param: *void)
{
    th := cast(*Thread) param
    th.userLambda(:th)
}

// Creates a new thread
func create(lambda: func(Thread), userParam: *void = null)->Thread
{
    debug.assert(lambda != null, "lambda parameter is null")

    var result: retval
    param := &result

    // As the threadEntry will be called by the os, need to be sure
    // we have a valid function pointer whatever the case is :
    // - we are running bytecode, and threadEntry should be run by the compiler
    // - we are native, and threadEntry is like a foreign function (native, i.e. callback)
    var paramBC: [2] *void
    var l64 = cast(u64) &threadEntry
    if l64 & swag.LambdaBcMarker
    {
        paramBC[0] = acast lambda
        paramBC[1] = acast &result
        param = acast &paramBC[0]
        l64 = acast @thrunptr()
    }
    else
    {
        l64 &= ~swag.LambdaForeignMarker
    }

    h := CreateThread(null, 0, cast(*void) l64, cast(*void) param, CREATE_SUSPENDED, &result.id)
    if h == INVALID_HANDLE_VALUE
        return result

    result.handle = h
    result.userLambda = lambda
    result.userParam = userParam
    return result
}

impl Thread
{
    // Resume the given thread, if it was paused
    func start(using self)->bool
    {
        debug.assert(isValid())
        return ResumeThread(handle) != cast(DWORD) -1
    }

    // Wait for the given thread to be done, and close it
    // After that call, isValid() will return false
    func wait(using self)
    {
        debug.assert(isValid())
        WaitForSingleObject(handle, INFINITE)
        CloseHandle(handle)
        @init(self)
    }

    // Returns true if the thread has finished
    func isDone(using self)->bool
    {
        debug.assert(isValid())
        return WaitForSingleObject(handle, 0) != WAIT_TIMEOUT
    }
}