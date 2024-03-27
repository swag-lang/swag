//#global skip
using Core

#test
{
    var str: String = "toto"

    var cpt = 0
    visit:Runes str
    {
        break
    }

    @assert(cpt == 0)

    visit:Runes str
    {
        cpt += 1
        continue
    }

    @assert(cpt == 4)
}
