#global public
#global namespace Time

// Represents an interval of time, stored as a 64-bit integer (in ticks)
struct TimeSpan
{
    ticks: Ticks // Duration represented in ticks
}

impl TimeSpan
{
    // Compares two TimeSpan values
    mtd opCmp(other: TimeSpan)->s32
    {
        return .ticks <=> other.ticks
    }

    // Returns the current time as a TimeSpan
    func now()->TimeSpan
    {
        let result: retval = from(DateTime.now())
        return result
    }

    // Sets the TimeSpan to the current time
    mtd setNow()
    {
        let dateTime = DateTime.now()
        .ticks = dateToTicks(dateTime.year, dateTime.month, dateTime.day)
        .ticks += timeToTicks(dateTime.hour, dateTime.minute, dateTime.second, dateTime.millisecond)
    }

    // Creates a TimeSpan from a DateTime
    func from(dateTime: DateTime)->TimeSpan
    {
        var result: retval = undefined
        result.ticks = dateToTicks(dateTime.year, dateTime.month, dateTime.day)
        result.ticks += timeToTicks(dateTime.hour, dateTime.minute, dateTime.second, dateTime.millisecond)
        return result
    }

    // Converts the TimeSpan to a DateTime
    mtd const toDateTime()->DateTime
    {
        var dateTime: retval = undefined
        (dateTime.year, dateTime.month, dateTime.day) = ticksToDate(.ticks)
        (dateTime.hour, dateTime.minute, dateTime.second, dateTime.millisecond) = ticksToTime(.ticks)
        return dateTime
    }

    // Adds or subtracts a number of years to the TimeSpan
    mtd addYears(years: s32)
    {
        Debug.assert(-10000 <= years and years <= 10000, "overflow")
        .addMonths(years * 12)
    }

    // Adds or subtracts a number of months to the TimeSpan
    mtd addMonths(months: s32)
    {
        Debug.assert(-120000 <= months and months <= 120000, "overflow")
        var (y, m, d) = ticksToDate(.ticks)
        let i = cast(s32) (m - 1) + months
        if i >= 0
        {
            m = cast(u16) (i % 12 + 1)
            y += cast(u16) (i / 12)
        }
        else
        {
            m = cast(u16) (12 + (i + 1) % 12)
            y += cast(u16) ((i - 11) / 12)
        }

        Debug.assert(1 <= y and y <= 9999, "overflow")
        let days = daysInMonth(y, m)
        if d > days do
            d = days
        .ticks = dateToTicks(y, m, d) + (.ticks % TicksPerDay)
    }

    // Adds or subtracts a number of days to the TimeSpan
    mtd addDays(days: s32)
    {
        if days > 0 do
            .add(cast(u32) days, MillisPerDay)
        else do
            .remove(cast(u32) -days, MillisPerDay)
    }

    // Adds or subtracts a number of hours to the TimeSpan
    mtd addHours(hours: s32)
    {
        if hours > 0 do
            .add(cast(u32) hours, MillisPerHour)
        else do
            .remove(cast(u32) -hours, MillisPerHour)
    }

    // Adds or subtracts a number of minutes to the TimeSpan
    mtd addMinutes(minutes: s32)
    {
        if minutes > 0 do
            .add(cast(u32) minutes, MillisPerMinute)
        else do
            .remove(cast(u32) -minutes, MillisPerMinute)
    }

    // Adds or subtracts a number of seconds to the TimeSpan
    mtd addSeconds(seconds: s32)
    {
        if seconds > 0 do
            .add(cast(u32) seconds, MillisPerSecond)
        else do
            .remove(cast(u32) -seconds, MillisPerSecond)
    }

    // Adds or subtracts a number of milliseconds to the TimeSpan
    mtd addMilliSeconds(milliSeconds: s32)
    {
        if milliSeconds > 0 do
            .add(cast(u32) milliSeconds, 1)
        else do
            .remove(cast(u32) -milliSeconds, 1)
    }

    // Returns the total duration in days
    mtd totalDays()->f64 => .ticks * (1.0'f64 / TicksPerDay)
    // Returns the total duration in hours
    mtd totalHours()->f64 => .ticks * (1.0'f64 / TicksPerHour)
    // Returns the total duration in minutes
    mtd totalMinutes()->f64 => .ticks * (1.0'f64 / TicksPerMinute)
    // Returns the total duration in seconds
    mtd totalSeconds()->f64 => .ticks * (1.0'f64 / TicksPerSecond)
    // Returns the total duration in milliseconds
    mtd totalMilliSeconds()->f64 => .ticks * (1.0'f64 / TicksPerMilliSecond)

    // Adds a time interval to the current TimeSpan
    internal mtd add(value: u32, scale: u64)
    {
        var millis = value * scale
        Debug.assert(millis <= MaxMillis, "overflow")
        millis *= TicksPerMilliSecond
        Debug.assert(millis <= (MaxTicks - .ticks), "overflow")
        .ticks += millis
    }

    // Subtracts a time interval from the current TimeSpan
    internal mtd remove(value: u32, scale: u64)
    {
        var millis = value * scale
        Debug.assert(millis <= MaxMillis, "overflow")
        millis *= TicksPerMilliSecond
        Debug.assert(millis <= .ticks, "ticks overflow")
        .ticks -= millis
    }
}
