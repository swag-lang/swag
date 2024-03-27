using Core

#test
{
    struct TT
    {
        val0: Array's32
    }

    var x: TT
    discard try CommandLine.parse(&x, ["-val0", "1", "2", "3"])
    @assert(x.val0.count == 3)
    @assert(x.val0[0] == 1)
    @assert(x.val0[1] == 2)
    @assert(x.val0[2] == 3)
}