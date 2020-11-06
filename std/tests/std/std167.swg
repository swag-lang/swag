#import "std"
using std.time

//#[swag.printbc]
#test
{
    @assert(DateTime.daysInMonth(2020, 1) == 31)
    @assert(DateTime.daysInMonth(2020, 2) == 29)

    ticks := DateTime.dateToTicks(2020, 11, 6)
    res := DateTime.ticksToDate(ticks)
    @assert(res.year == 2020)
    @assert(res.month == 11)
    @assert(res.day == 6)
}