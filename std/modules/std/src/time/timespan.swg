public namespace std.time;

struct TimeSpan
{
    ticks: Ticks
}

impl TimeSpan
{
    // Creates a TimeSpan with the given DateTime
    func from(dateTime: DateTime)->TimeSpan
    {
        result := retval
        using result, dateTime
        ticks = dateToTicks(year, month, day)
        ticks += timeToTicks(hour, minute, second, millisecond)
        return result
    }

    // Converts a TimeSpan to a DateTime
    func toDateTime(using self)->DateTime
    {
        dateTime := retval
        using dateTime
        (year, month, day) = ticksToDate(ticks)
        (hour, minute, second, millisecond) = ticksToTime(ticks)
        return dateTime
    }
}