//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    var toto = 0
    //@breakpoint()
    var x = closure|ref toto|()
    {
        toto += 1
    }

    loop 5
        x()
    @assert(toto == 5)
}