#if WINDOWS;
namespace time;
using win32

impl DateTime
{
    func fromSYSTEMTIME(using self, systemTime: SYSTEMTIME)
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
        var systemTime: SYSTEMTIME = ?
        GetLocalTime(&systemTime)

        var result: retval = ?
        result.fromSYSTEMTIME(systemTime)
        return result
    }

    // Initialize the structure with the current local date and time
    func setNow(using self)
    {
        var systemTime: SYSTEMTIME = ?
        GetLocalTime(&systemTime)
        fromSYSTEMTIME(systemTime)
    }
}