#global #if WINDOWS
#global namespace Time
using Win32

impl DateTime
{
    mtd setSYSTEMTIME(systemTime: SYSTEMTIME)
    {
        .day         = systemTime.wDay
        .month       = systemTime.wMonth
        .year        = systemTime.wYear
        .hour        = systemTime.wHour
        .minute      = systemTime.wMinute
        .second      = systemTime.wSecond
        .millisecond = systemTime.wMilliseconds
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
        .setSYSTEMTIME(systemTime)
    }
}
