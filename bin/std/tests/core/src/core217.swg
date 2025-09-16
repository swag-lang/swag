//#global skip
using Core

#test
{
    let str: String = "toto"

    var cpt = 0
    foreach #Runes str
    {
        break
    }

    @assert(cpt == 0)

    foreach #Runes str
    {
        cpt += 1
        continue
    }

    @assert(cpt == 4)
}
