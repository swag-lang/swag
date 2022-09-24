using Core, Time

//#[Swag.PrintBc]
#test
{
    {
        str := "1971-13-05 12:24:08"
        (x1, eat) := catch DateTime.parse(str, .Iso)
        @assert(@err != null)
    }

    {
        str := "Monday September 5 1971 12:24:08"
        (x1, eat) := catch DateTime.parse(str, .DateTime)
        @assert(@err != null)
    }

    {
        str := "Sunday September 5 1971 12:24:68"
        (x1, eat) := catch DateTime.parse(str, .DateTime)
        @assert(@err != null)
    }
}