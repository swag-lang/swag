#global skip // remove typeset from language the 21 july 2021
#test
{
    typeset XX
    {
        Bool {bool}
        Int  {s64}
    }

    var x: XX = Bool{true}
    var G = 0
    switch x
    {
    case Bool:
        G = 1
        break
    case Int:
        @assert(false)
        break
    }

    @assert(G == 1)
}