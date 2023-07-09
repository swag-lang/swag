using Core

#test
{
    struct TT
    {
        count:  s8
    }

    var x: TT
    var a0 = catch CommandLine.parse(&x, ["-count", "-count"])
    @assert(@errmsg != null)

    var a1 = catch CommandLine.parse(&x, ["-count:3000"])
    @assert(@errmsg != null)

    var a2 = catch CommandLine.parse(&x, ["-toto"])
    @assert(@errmsg != null)
}