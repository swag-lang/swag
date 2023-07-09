
using Core.Time

//#[Swag.PrintBc]
#test
{
    @assert(daysInMonth(2020, 1) == 31)
    @assert(daysInMonth(2020, 2) == 29)

    {
        var ticks = dateToTicks(2020, 11, 6)
        var res = ticksToDate(ticks)
        @assert(res.year == 2020)
        @assert(res.month == 11)
        @assert(res.day == 6)
    }

    {
        var ticks = timeToTicks(13, 58, 10, 100)
        var res1 = ticksToTime(ticks)
        @assert(res1.hour == 13)
        @assert(res1.minute == 58)
        @assert(res1.second == 10)
        @assert(res1.millisecond == 100)
    }
}