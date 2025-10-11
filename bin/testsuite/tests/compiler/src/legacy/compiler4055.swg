//#[Swag.PrintBc]
//#[Swag.Optimize("", false)]
#test
{
    var toto = 0
    //@breakpoint()
    let x = func|&toto|()
    {
        toto += 1
    }

    for 5:
        x()
    @assert(toto == 5)
}
