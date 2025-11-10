//#[Swag.PrintBc]
#test
{
    var cpt = 0
    for var i = 0; i < 10; i += 1
    {
        if i >= 5 do
            continue
        cpt += 1
    }

    @assert(cpt == 5)
}
