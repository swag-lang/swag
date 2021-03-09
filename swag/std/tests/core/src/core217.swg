
using core

#test
{
    var str: String = "toto"

    cpt := 0
    visit str
    {
        break
    }

    @assert(cpt == 0)

    visit str
    {
        cpt += 1
        continue
    }

    @assert(cpt == 4)
}