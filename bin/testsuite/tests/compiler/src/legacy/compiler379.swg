//#[Swag.PrintBc]
#global testerror "Err0297"
#test
{
    var cpt = 0

    for -10
    {
        cpt += 1; }

    @assert(cpt == 10)
}
