
//#[Swag.PrintBc]
#test
{
    var cpt = 0
    for var i = 0; i < 10; i += 1
    {
        cpt += 1
        break
    }

    @assert(cpt == 1)
}
