using Core, Time

//#[Swag.PrintBc]
#test
{
    var x: DateTime{year = 1971, month = 9, day = 5, hour = 12, minute = 24, second = 8, millisecond = 666}

    {
        str := x.toString(.Iso)
        @assert(str == "1971-09-05 12:24:08")
        (x1, eat) := try DateTime.parse(str, .Iso)
        x1.millisecond = x.millisecond
        @assert(x1 == x)
    }

    {
        str := x.toString(.IsoMs)
        @assert(str == "1971-09-05 12:24:08.666")
        (x1, eat) := try DateTime.parse(str, .IsoMs)
        @assert(x1 == x)
    }

    {
        str := x.toString(.DateTime)
        @assert(str == "Sunday September 5 1971 12:24:08")
        (x1, eat) := try DateTime.parse(str, .DateTime)
        x1.millisecond = x.millisecond
        @assert(x1 == x)
    }

    {
        str := x.toString(.DateTimeMs)
        @assert(str == "Sunday September 5 1971 12:24:08.666")
        (x1, eat) := try DateTime.parse(str, .DateTimeMs)
        @assert(x1 == x)
    }
}