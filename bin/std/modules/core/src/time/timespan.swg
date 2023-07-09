#global public
#global namespace Time

// Represents an interval of time, stored as a 64 bits integer
struct TimeSpan
{
    ticks: Ticks
}

impl TimeSpan
{
    mtd opCmp(other: Self)->s32
    {
        return ticks <=> other.ticks
    }

    // Returns the current date and time
    func now()->TimeSpan
    {
        var result: retval = from(DateTime.now())
        return result
    }

    // Initialize the TimeSpan with the current date and time
    mtd setNow()
    {
        var dateTime = DateTime.now()
        using dateTime
        ticks = dateToTicks(year, month, day)
        ticks += timeToTicks(hour, minute, second, millisecond)
    }

    // Creates a TimeSpan with the given DateTime
    func from(dateTime: DateTime)->TimeSpan
    {
        var result: retval = undefined
        using result, dateTime
        ticks = dateToTicks(year, month, day)
        ticks += timeToTicks(hour, minute, second, millisecond)
        return result
    }

    // Converts a TimeSpan to a DateTime
    mtd toDateTime()->DateTime
    {
        var dateTime: retval = undefined
        using dateTime
        (year, month, day) = ticksToDate(ticks)
        (hour, minute, second, millisecond) = ticksToTime(ticks)
        return dateTime
    }

    // Add or remove the given amount of years to the TimeSpan
    mtd addYears(years: s32)
    {
        Debug.assert(-10000 <= years and years <= 10000, "overflow")
        addMonths(years * 12)
    }

    // Add or remove the given amount of months to the TimeSpan
    mtd addMonths(months: s32)
    {
        Debug.assert(-120000 <= months and months <= 120000, "overflow")
        var (y, m, d) = ticksToDate(ticks)
        var i = cast(s32) (m - 1) + months
        if i >= 0
        {
            m =  cast(u16) (i % 12 + 1)
            y += cast(u16) (i / 12)
        }
        else
        {
            m =  cast(u16) (12 + (i + 1) % 12)
            y += cast(u16) ((i - 11) / 12)
        }

        Debug.assert(1 <= y and y <= 9999, "overflow")
        var days = daysInMonth(y, m)
        if (d > days) d = days
        ticks = dateToTicks(y, m, d) + (ticks % TicksPerDay)
    }

    // Add or remove the given amount of days to the TimeSpan
    mtd addDays(days: s32)
    {
        if days > 0
            add(cast(u32) days, MillisPerDay)
        else
            remove(cast(u32) -days, MillisPerDay)
    }

    // Add or remove the given amount of hours to the TimeSpan
    mtd addHours(hours: s32)
    {
        if hours > 0
            add(cast(u32) hours, MillisPerHour)
        else
            remove(cast(u32) -hours, MillisPerHour)
    }

    // Add or remove the given amount of minutes to the TimeSpan
    mtd addMinutes(minutes: s32)
    {
        if minutes > 0
            add(cast(u32) minutes, MillisPerMinute)
        else
            remove(cast(u32) -minutes, MillisPerMinute)
    }

    // Add or remove the given amount of minutes to the TimeSpan
    mtd addSeconds(seconds: s32)
    {
        if seconds > 0
            add(cast(u32) seconds, MillisPerSecond)
        else
            remove(cast(u32) -seconds, MillisPerSecond)
    }

    // Add or remove the given amount of minutes to the TimeSpan
    mtd addMilliSeconds(milliSeconds: s32)
    {
        if milliSeconds > 0
            add(cast(u32) milliSeconds, 1)
        else
            remove(cast(u32) -milliSeconds, 1)
    }

    mtd totalDays()         => ticks * (1.0'f64 / TicksPerDay)
    mtd totalHours()        => ticks * (1.0'f64 / TicksPerHour)
    mtd totalMinutes()      => ticks * (1.0'f64 / TicksPerMinute)
    mtd totalSeconds()      => ticks * (1.0'f64 / TicksPerSecond)
    mtd totalMilliSeconds() => ticks * (1.0'f64 / TicksPerMilliSecond)

    private mtd add(value: u32, scale: u64)
    {
        var millis = value * scale
        Debug.assert(millis <= MaxMillis, "overflow")
        millis *= TicksPerMilliSecond
        Debug.assert(millis <= (MaxTicks - ticks), "overflow")
        ticks += millis
    }

    private mtd remove(value: u32, scale: u64)
    {
        var millis = value * scale
        Debug.assert(millis <= MaxMillis, "overflow")
        millis *= TicksPerMilliSecond
        Debug.assert(millis <= ticks, "ticks overflow")
        ticks -= millis
    }
}