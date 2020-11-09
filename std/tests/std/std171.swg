#import "std"
using std.time

#test
{
    sp := TimeSpan.from(DateTime{2020, 11, 8})
    sp.addHours(1)
    @assert(sp.toDateTime().hour == 1)
    sp.addHours(24)
    @assert(sp.toDateTime().day == 9)
    sp.addHours(-24)
    @assert(sp.toDateTime().day == 8)
}