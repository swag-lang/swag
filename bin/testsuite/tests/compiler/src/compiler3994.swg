struct A {}

func toto(x: any)
{
    k := @kindof(x)
    @assert(k.kind == .Pointer)
}

//#[Swag.PrintBc]
func titi(a: const ref A)
{
    toto(a)
}

#test
{
    var a: A
    titi(a)
}