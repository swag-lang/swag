using Core, Core.CommandLine

#test
{
    struct TT
    {
        #[ArgParams(nameAlias: "f")]
        val0: s32
    }

    var x: TT
    discard try parse(&x, ["-f:1"])
    @assert(x.val0 == 1)
}
