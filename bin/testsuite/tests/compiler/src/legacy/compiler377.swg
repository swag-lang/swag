#global testerror "Err0295"
#test
{
    var cpt = 0

    for 10.0
    {
        cpt += 1; }

    @assert(cpt == 10)
}
