public namespace std.time;

struct TimeSpan
{
    ticks: Ticks
}

impl TimeSpan
{
    func from(dateTime: DateTime)->TimeSpan
    {
        var result: TimeSpan = ?
        using result, dateTime
        ticks = dateToTicks(year, month, day)
        ticks += timeToTicks(hour, minute, second, millisecond)
        return result
    }

    #[swag.property]
    func toDateTime(using self)->DateTime
    {
        var dateTime: DateTime = ?
        using dateTime
        (year, month, day) = ticksToDate(ticks)
        (hour, minute, second, millisecond) = ticksToTime(ticks)
        return dateTime
    }
}