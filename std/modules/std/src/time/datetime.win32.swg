#if std.WINDOWS;
using win32
namespace std.time;

protected impl DateTime
{
    func fromSYSTEMTIME(using self, systemTime: SYSTEMTIME)
    {
        using systemTime
        day         = wDay
        dayOfWeek   = wDayOfWeek
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
    // Initialize the structure with the current local date and time
    func now(using self)
    {
        var systemTime: SYSTEMTIME = ?
        GetLocalTime(&systemTime)
        self.fromSYSTEMTIME(systemTime)
    }
}