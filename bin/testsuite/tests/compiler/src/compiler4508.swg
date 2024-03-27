struct T
{
    a, b: s32
}

//#[Swag.PrintBc]
#[Swag.NoInline]
func toto(t: T)->T
{
    //@print(t.a, " ", t.b, "\n")
    return t
}

//#[Swag.Optim("", false)]
#test
{
    var x = toto({1, 2})
    @assert(x.a == 1)
    @assert(x.b == 2)
}
