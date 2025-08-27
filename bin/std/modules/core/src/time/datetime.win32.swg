#global marked
#global #if WINDOWS
#global namespace Time
using Win32

impl DateTime
{
    mtd setSYSTEMTIME(systemTime: SYSTEMTIME)
    {
        me.day         = systemTime.wDay
        me.month       = systemTime.wMonth
        me.year        = systemTime.wYear
        me.hour        = systemTime.wHour
        me.minute      = systemTime.wMinute
        me.second      = systemTime.wSecond
        me.millisecond = systemTime.wMilliseconds
    }
}

public impl DateTime
{
    // Returns a DateTime containing the current date and time
    func now()->DateTime
    {
        var systemTime: SYSTEMTIME = undefined
        GetLocalTime(&systemTime)

        var result: retval = undefined
        result.setSYSTEMTIME(systemTime)
        return result
    }

    // Initialize the structure with the current local date and time
    mtd setNow()
    {
        var systemTime: SYSTEMTIME = undefined
        GetLocalTime(&systemTime)
        me.setSYSTEMTIME(systemTime)
    }
}
