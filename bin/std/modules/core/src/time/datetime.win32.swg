#global #if WINDOWS
#global namespace Time
using Win32

impl DateTime
{
    mtd setSYSTEMTIME(systemTime: SYSTEMTIME)
    {
        using systemTime
        day         = wDay
        month       = wMonth
        year        = wYear
        hour        = wHour
        minute      = wMinute
        second      = wSecond
        millisecond = wMilliseconds
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
        setSYSTEMTIME(systemTime)
    }
}
