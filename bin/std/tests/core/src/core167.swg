using Core.Time

//#[Swag.PrintBc]
#test
{
    @assert(daysInMonth(2020, 1) == 31)
    @assert(daysInMonth(2020, 2) == 29)

    {
        let ticks = dateToTicks(2020, 11, 6)
        let res   = ticksToDate(ticks)
        @assert(res.year == 2020)
        @assert(res.month == 11)
        @assert(res.day == 6)
    }

    {
        let ticks = timeToTicks(13, 58, 10, 100)
        let res1  = ticksToTime(ticks)
        @assert(res1.hour == 13)
        @assert(res1.minute == 58)
        @assert(res1.second == 10)
        @assert(res1.millisecond == 100)
    }
}
