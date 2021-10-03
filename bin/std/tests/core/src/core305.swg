using Core

#test
{
    enum EE { R; G; B; }
    var x: EE = ?
    discard try Parse.pokeValue(&x, EE, "B")
    @assert(x == .B)
}