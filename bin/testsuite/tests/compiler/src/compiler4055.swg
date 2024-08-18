//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    var toto = 0
    //@breakpoint()
    let x = closure|&toto|()
    {
        toto += 1
    }

    loop 5:
        x()
    @assert(toto == 5)
}