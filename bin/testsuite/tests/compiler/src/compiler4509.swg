struct T { a, b: s32; }

//#[Swag.PrintBc]
#[Swag.NoInline]
func toto()->T
{
    //@print(t.a, " ", t.b, "\n")
    return {1, 2}
}

//#[Swag.Optimize("", false)]
#test
{
    var x = toto()
    @assert(x.a == 1)
    @assert(x.b == 2)
}