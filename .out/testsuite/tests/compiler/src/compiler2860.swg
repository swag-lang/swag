#global skip // remove typeset from language the 21 july 2021
#global testerror

#test
{
    typeset XX
    {
        Bool {bool}
        Int  {int}
    }

    var x: XX = Bool{true}
    G := 0
    switch x
    {
    case Bool:
        break
    case Bool:
        G = 1
        break
    case Int:
        @assert(false)
        break
    }

    @assert(G == 1)
}