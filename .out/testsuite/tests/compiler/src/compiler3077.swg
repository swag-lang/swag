struct A
{
    a: s32 = 665
    b: s32 = 666
}

//#[Swag.PrintBc]
func toto(x: any)
{
    v := cast(A) x
    @assert(v.a == 665)
    @assert(v.b == 666)
}

#test
{
    x := A{}
    toto(x)
}