using Core

#test
{
    enum EE { R; G; B; }
    var x: EE = ?
    discard try StrConv.pokeValue(&x, EE, "B")
    @assert(x == .B)
}