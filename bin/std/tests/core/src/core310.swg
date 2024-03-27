//#global skip
using Core

#test
{
    struct TT
    {
        val0: Array'String
    }

    var x: TT
    discard try CommandLine.parse(&x, ["-val0", #""a""#, #""b and c""#, "cc and dd"])
    @assert(x.val0.count == 3)
    @assert(x.val0[0] == "a")
    @assert(x.val0[1] == "b and c")
    @assert(x.val0[2] == "cc and dd")
}