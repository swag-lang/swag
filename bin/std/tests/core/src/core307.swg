using Core

#test
{
    struct TT
    {
        val0: string
    }

    var x: TT
    discard try CommandLine.parse(&x, @["-val0", @""toto la foule""@])
    @assert(x.val0 == "toto la foule")
}