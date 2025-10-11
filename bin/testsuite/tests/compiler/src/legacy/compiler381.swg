//#[Swag.PrintBc]
#test
{
    let toto = "1234"

    var cpt = 0
    for @countof(toto)
    {
        //@print("toto");
        cpt += 1; if (cpt == 2) do
            break
    }

    //@print(cpt);
    @assert(cpt == 2)
}
