//#[Swag.PrintBc]
//#[Swag.Optim("", false)]
#test
{
    toto := 0
    //@breakpoint()
    x := closure|ref toto|()
    {
        toto += 1
    }

    loop 5
        x()
    @assert(toto == 5)
}