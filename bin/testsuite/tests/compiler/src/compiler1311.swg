//#[Swag.PrintBc]
/*#test
{
    var cpt = 0

    #scope outer loop x in 10
    {
        cpt += 1
        loop 10
        {
            continue outer
        }

        cpt += 1
    }

	@assert(cpt == 10)
}*/