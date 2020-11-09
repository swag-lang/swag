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
        addTicks(millis * TicksPerMilliSecond)
    }

    private func remove(using self, value: u32, scale: u64)
    {
        millis := value * scale
        debug.assert(millis <= MaxMillis, "overflow")
        removeTicks(millis * TicksPerMilliSecond)
    }

    private func addTicks(using self, value: u64)
    {
        debug.assert(value <= (MaxTicks - ticks), "overflow")
        ticks += value
    }

    private func removeTicks(using self, value: u64)
    {
        debug.assert(value <= ticks, "ticks overflow")
        ticks -= value
    }
}