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

    loop 5 do
        x()
    @assert(toto == 5)
}