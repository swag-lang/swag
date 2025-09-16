using Core

//#[Swag.PrintBc]
#test
{
    let str: String = "toto"
    var cpt  = 0
    foreach #Runes str
    {
        cpt += 1
    }

    @assert(cpt == 4)
}
