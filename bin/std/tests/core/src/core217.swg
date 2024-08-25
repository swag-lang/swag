//#global skip
using Core

#test
{
    var str: String = "toto"

    var cpt = 0
    foreach<Runes> str
    {
        break
    }

    @assert(cpt == 0)

    foreach<Runes> str
    {
        cpt += 1
        continue
    }

    @assert(cpt == 4)
}
