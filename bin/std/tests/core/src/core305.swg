using Core

#test
{
    enum EE
    {
        R
        G
        B
    }
    var x: EE = undefined
    discard try StrConv.parseValue(&x, EE, "B")
    @assert(x == .B)
}
