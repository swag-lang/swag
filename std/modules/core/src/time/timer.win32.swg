#if #os == "windows";
public namespace time;
using win32

#[swag.callback]
private func timerEntry(param: *void)
{
    @print("entry\n")
}

impl Timer
{
    // Creates a new timer
    func init(using self, autoReset: bool, elapsedTimeMs: u32, lambda: func(Timer))->ErrorId
    {
        h := CreateWaitableTimerA(null, autoReset ? TRUE : FALSE, null)
        if h == null
        {
            error.setLastErrorId()
            return error.last()
        }

        // As the threadEntry will be called by the os, need to be sure
        // we have a valid function pointer whatever the case is :
        // - we are running bytecode, and threadEntry should be run by the compiler
        // - we are native, and threadEntry is like a foreign function (native, i.e. callback)
        var l64 = cast(uint) &timerEntry
        if l64 & swag.LambdaBcMarker
            l64 = autocast @thrunptr()
        else
            l64 &= ~swag.LambdaForeignMarker

        @print(cast(s64) l64)

        // Setup timer
        var t: s64 = cast(s64) elapsedTimeMs * -10000's64
        if !SetWaitableTimer(h, &t, 0, cast(*void) l64, self, FALSE)
        {
            CloseHandle(h)
            error.setLastErrorId()
            return error.last()
        }

        handle = h
        SleepEx(2000, TRUE)
        return null
    }

    // Release the timer
    func release(using self)
    {
        if !handle
            return
        CancelWaitableTimer(handle)
        CloseHandle(handle)
        handle = null
    }
}