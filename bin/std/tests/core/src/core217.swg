//#global skip
using Core

#test
{
    var str: String = "toto"

    var cpt = 0
    visit:runes str
    {
        break
    }

    @assert(cpt == 0)

    visit:runes str
    {
        cpt += 1
        continue
    }

    @assert(cpt == 4)
}