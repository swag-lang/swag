struct A
{
    a: s32 = 665
    b: s32 = 666
}

//#[Swag.PrintBc]
func toto(x: any)
{
    var v = cast(A) x
    @assert(v.a == 665)
    @assert(v.b == 666)
}

#test
{
    var x = A{}
    toto(x)
}