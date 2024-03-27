
func(T) valueof(x: any)->T
{
    let ptr = cast(*T) @dataof(x)
    return dref ptr
}

//#[Swag.PrintBc]
func toto(xx: string, a: ...)
{
    @assert(valueof'string(a[0]) == "toto")
    @assert(valueof's32(a[1]) == 10)
}

//#[Swag.PrintBc]
#test
{
    toto("%s%d", "toto", 10)
}
