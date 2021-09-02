
using Core


//#[Swag.PrintBc]
#test
{
    var str: String = "toto"
    cpt := 0
    visit str
    {
        cpt += 1
    }

    @assert(cpt == 4)
}
