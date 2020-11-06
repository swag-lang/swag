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
    func getDateTime(using self)->DateTime
    {
        var dateTime: DateTime = ?
        res := ticksToDate(ticks)
        dateTime.year = res.year
        dateTime.month = res.month
        dateTime.day = res.day
        res1 := ticksToTime(ticks)
        dateTime.hour = res1.hour
        dateTime.minute = res1.minute
        dateTime.second = res1.second
        dateTime.millisecond = res1.millisecond
        return dateTime
    }
}