using Core

#test
{
    enum EE
    {
        R
        G
        B
    }
    struct TT
    {
        val0: EE = undefined
    }

    var x: TT
    discard try CommandLine.parse(&x, ["-val0:G"])
    @assert(x.val0 == .G)
    discard try CommandLine.parse(&x, ["-val0:B"])
    @assert(x.val0 == .B)
}
