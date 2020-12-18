#import "core"
using core.time

#test
{
    {
        var x: DateTime{2020, 11, 8}
        @assert(x.dayOfWeek == DayOfWeek.Sunday)
    }
    {
        var x: DateTime{2019, 7, 3}
        @assert(x.dayOfWeek == DayOfWeek.Wednesday)
    }
    {
        var x: DateTime{1971, 9, 5}
        @assert(x.dayOfWeek == DayOfWeek.Sunday)
    }
}