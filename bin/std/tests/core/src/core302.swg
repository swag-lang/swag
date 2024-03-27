using Core

#test
{
    struct TT
    {
        count: bool
    }

    var x: TT
    discard try CommandLine.parse(&x, ["-count"])
    @assert(x.count)
    discard try CommandLine.parse(&x, ["-count:true"])
    @assert(x.count)
    discard try CommandLine.parse(&x, ["-count=false"])
    @assert(!x.count)
}
