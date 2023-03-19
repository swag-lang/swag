#global testerror
//#[Swag.PrintBc]
#test
{
    cpt := 0

    #macro
    {
        loop 10 #up cpt += 1
    }

    @assert(cpt == 10)
}