public namespace std.time;

struct TimeSpan
{
    ticks: Ticks
}

impl TimeSpan
{
    func from(using self, dateTime: DateTime)
    {
        ticks = dateToTicks(dateTime.year, dateTime.month, dateTime.day)
        ticks += timeToTicks(dateTime.hour, dateTime.minute, dateTime.second, dateTime.millisecond)
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