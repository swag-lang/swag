using Core, Core.Time

//#[Swag.PrintBc]
#test
{
    var x: DateTime{year: 1971, month: 9, day: 5, hour: 12, minute: 24, second: 8, millisecond: 666}

    {
        var str = x.toString(.DateTimeIso)
        @assert(str == "1971-09-05 12:24:08")
        var (x1, eat) = try DateTime.parse(str)
        x1.millisecond = x.millisecond
        @assert(x1 == x)
    }

    {
        var str = x.toString(.DateTimeIsoMs)
        @assert(str == "1971-09-05 12:24:08.666")
        var (x1, eat) = try DateTime.parse(str)
        @assert(x1 == x)
    }

    {
        var str = x.toString(.DateTime)
        @assert(str == "Sunday September 5 1971 12:24:08")
        var (x1, eat) = try DateTime.parse(str)
        x1.millisecond = x.millisecond
        @assert(x1 == x)
    }

    {
        var str = x.toString(.DateTimeMs)
        @assert(str == "Sunday September 5 1971 12:24:08.666")
        var (x1, eat) = try DateTime.parse(str)
        @assert(x1 == x)
    }
}
