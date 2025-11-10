//#[Swag.PrintBc]

func(T) toto(x: [1] *T)
{
    @assert(dref x[0] == 10)
}

#test
{
    var cpt: [?] s32 = [10]
    var ptr: [?] *s32 = [&cpt]
    toto(ptr)
}
