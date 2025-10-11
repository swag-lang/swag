//#[Swag.PrintBc]

func(T) toto(x: *T)
{
    @assert(dref x == 666)
}

#test
{
    var cpt = 666
    toto(&cpt)
}
