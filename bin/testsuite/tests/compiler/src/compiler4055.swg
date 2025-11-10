//#[Swag.PrintBc]
//#[Swag.Optimize(.All, false)]
#test
{
    var toto = 0
    //@breakpoint()
    let x = func|&toto|()
    {
        toto += 1
    }

    for 5 do
        x()
    @assert(toto == 5)
}
