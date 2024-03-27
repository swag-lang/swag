func(T) valueof(x: any)->T
{
    let ptr = cast(*T) @dataof(x)
    return dref ptr
}

//#[Swag.PrintBc]
func toto(xx: string, a: ...)
{
    let x0 = valueof's32(a[0])
    @assert(x0 == 10)
    let x1 = valueof'string(a[1])
    @assert(x1 == "toto")
    let x2 = valueof's32(a[2])
    @assert(x2 == 600)
}

//#[Swag.PrintBc]
#test
{
    toto("true", 10, "toto", 600)
}
