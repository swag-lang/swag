public namespace std.time;

struct TimeSpan
{
    ticks: Ticks
}

impl TimeSpan
{
    //#[swag.printbc]
    func from(dateTime: DateTime)->TimeSpan
    {
        result := retval
        using result, dateTime
        ticks = dateToTicks(year, month, day)
        ticks += timeToTicks(hour, minute, second, millisecond)
        return result
    }

    //#[swag.printbc]
    func toDateTime(using self)->DateTime
    {
        dateTime := retval
        using dateTime
        (year, month, day) = ticksToDate(ticks)
        console.print("%\n", year)
        (hour, minute, second, millisecond) = ticksToTime(ticks)
        return dateTime
    }
}