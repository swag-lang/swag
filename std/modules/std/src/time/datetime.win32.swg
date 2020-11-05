#if std.WINDOWS;
using win32
namespace std.time;

impl DateTime
{
    // Initialize the structure with the current local date and time
    public func now(using self)
    {
        var systemTime: SYSTEMTIME = ?
        GetLocalTime(&systemTime)
        self.fromSYSTEMTIME(systemTime)
    }

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