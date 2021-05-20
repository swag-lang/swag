
using core.Time

//#[Swag.printbc]
#test
{
    @assert(daysInMonth(2020, 1) == 31)
    @assert(daysInMonth(2020, 2) == 29)

    {
        ticks := dateToTicks(2020, 11, 6)
        res := ticksToDate(ticks)
        @assert(res.year == 2020)
        @assert(res.month == 11)
        @assert(res.day == 6)
    }

    {
        ticks := timeToTicks(13, 58, 10, 100)
        res1 := ticksToTime(ticks)
        @assert(res1.hour == 13)
        @assert(res1.minute == 58)
        @assert(res1.second == 10)
        @assert(res1.millisecond == 100)
    }
}