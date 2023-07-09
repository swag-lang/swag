using Core, Time

//#[Swag.PrintBc]
#test
{
    {
        var str = "1971-13-05 12:24:08"
        var (x1, eat) = catch DateTime.parse(str, .Iso)
        @assert(@errmsg != null)
    }

    {
        var str = "Monday September 5 1971 12:24:08"
        var (x1, eat) = catch DateTime.parse(str, .DateTime)
        @assert(@errmsg != null)
    }

    {
        var str = "Sunday September 5 1971 12:24:68"
        var (x1, eat) = catch DateTime.parse(str, .DateTime)
        @assert(@errmsg != null)
    }
}