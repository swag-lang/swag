#if #os == "windows";
public namespace time;
using win32

impl Timer
{
    // Creates a new timer
    func create(autoReset: bool, elapsedTimeMs: u32)->{timer: Timer, err: ErrorId}
    {
        var result: retval

        h := CreateWaitableTimerA(null, autoReset ? TRUE : FALSE, null)
        if h == null
        {
            error.setLastErrorId()
            result.err = error.last()
            return result
        }

        result.timer.handle = h
        return result
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