using Core

#test
{
    struct TT
    {
        count:  s8
    }

    var x: TT
    a0 := catch CommandLine.parse(&x, @["-count", "-count"])
    @assert(@err != null)

    a1 := catch CommandLine.parse(&x, @["-count:3000"])
    @assert(@err != null)

    a2 := catch CommandLine.parse(&x, @["-toto"])
    @assert(@err != null)
}