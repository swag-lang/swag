//#[Swag.PrintBc]
/*#test
{
    var cpt = 0

    #scope(outer) for x in 10
    {
        cpt += 1
        for 10
        {
            continue outer
        }

        cpt += 1
    }

	@assert(cpt == 10)
}*/
