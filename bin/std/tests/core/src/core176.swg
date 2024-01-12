using Core


//#[Swag.PrintBc]
#test
{
    var str: String = "toto"
    var cpt = 0
    visit:runes str
    {
        cpt += 1
    }

    @assert(cpt == 4)
}
