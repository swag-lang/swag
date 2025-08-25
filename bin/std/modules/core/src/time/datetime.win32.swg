#global marked
#global #if WINDOWS
#global namespace Time
using Win32

impl DateTime
{
    mtd setSYSTEMTIME(systemTime: SYSTEMTIME)
    {
        using systemTime
        me.day         = wDay
        me.month       = wMonth
        me.year        = wYear
        me.hour        = wHour
        me.minute      = wMinute
        me.second      = wSecond
        me.millisecond = wMilliseconds
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
