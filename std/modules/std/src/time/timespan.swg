public namespace std.time;

// Represents an interval of time, stored as a 64 bits integer
struct TimeSpan
{
    ticks: Ticks
}

impl TimeSpan
{
    // Creates a TimeSpan with the given DateTime
    func from(dateTime: DateTime)->TimeSpan
    {
        var result: retval = ?
        using result, dateTime
        ticks = dateToTicks(year, month, day)
        ticks += timeToTicks(hour, minute, second, millisecond)
        return result
    }

    // Converts a TimeSpan to a DateTime
    func toDateTime(using self)->DateTime
    {
        var dateTime: retval = ?
        using dateTime
        (year, month, day) = ticksToDate(ticks)
        (hour, minute, second, millisecond) = ticksToTime(ticks)
        return dateTime
    }

    // Add or remove the given amount of years to the TimeSpan
    func addYears(using self, years: s32)
    {
        debug.assert(-10000 <= years <= 10000, "overflow")
        addMonths(years * 12)
    }

    // Add or remove the given amount of months to the TimeSpan
    func addMonths(using self, months: s32)
    {
        debug.assert(-120000 <= months <= 120000, "overflow")
        (y, m, d) := ticksToDate(ticks)
        i := cast(s32) (m - 1) + months
        if i >= 0
        {
            m = cast(u16) ((i % 12) + 1)
            y += cast(u16) (i / 12)
        }
        else
        {
            m = cast(u16) (12 + ((i + 1) % 12))
            y += cast(u16) ((i - 11) / 12)
        }

        debug.assert(1 <= y <= 9999, "overflow")
        days := daysInMonth(y, m)
        if (d > days) d = days
        ticks = dateToTicks(y, m, d) + (ticks % TicksPerDay)
    }

    // Add or remove the given amount of days to the TimeSpan
    func addDays(using self, days: s32)
    {
        if days > 0
            add(cast(u32) days, MillisPerDay)
        else
            remove(cast(u32) -days, MillisPerDay)
    }

    // Add or remove the given amount of hours to the TimeSpan
    func addHours(using self, hours: s32)
    {
        if hours > 0
            add(cast(u32) hours, MillisPerHour)
        else
            remove(cast(u32) -hours, MillisPerHour)
    }

    // Add or remove the given amount of minutes to the TimeSpan
    func addMinutes(using self, minutes: s32)
    {
        if minutes > 0
            add(cast(u32) minutes, MillisPerMinute)
        else
            remove(cast(u32) -minutes, MillisPerMinute)
    }

    // Add or remove the given amount of minutes to the TimeSpan
    func addSeconds(using self, seconds: s32)
    {
        if seconds > 0
            add(cast(u32) seconds, MillisPerSecond)
        else
            remove(cast(u32) -seconds, MillisPerSecond)
    }

    // Add or remove the given amount of minutes to the TimeSpan
    func addMilliSeconds(using self, milliSeconds: s32)
    {
        if milliSeconds > 0
            add(cast(u32) milliSeconds, 1)
        else
            remove(cast(u32) -milliSeconds, 1)
    }

    private func add(using self, value: u32, scale: u64)
    {
        millis := value * scale
        debug.assert(millis <= MaxMillis, "overflow")
        millis *= TicksPerMilliSecond
        debug.assert(millis <= (MaxTicks - ticks), "overflow")
        ticks += millis
    }

    private func remove(using self, value: u32, scale: u64)
    {
        millis := value * scale
        debug.assert(millis <= MaxMillis, "overflow")
        millis *= TicksPerMilliSecond
        debug.assert(millis <= ticks, "ticks overflow")
        ticks -= millis
    }
}