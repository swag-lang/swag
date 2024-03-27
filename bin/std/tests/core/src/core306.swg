using Core

#test
{
    #[Swag.EnumFlags]
    enum EE { R = 0; G; B; }
    struct TT
    {
        val0: EE = R
    }

    var x: TT
    discard try CommandLine.parse(&x, ["-val0:G", "-val0:B"])
    @assert(x.val0 == .G | .B)
}