func(T) valueof(x: any)->T
{
    let ptr = cast(*T) @dataof(x)
    return dref ptr
}

//#[Swag.PrintBc]
#test
{
    let x: any = 666
    @assert(valueof's32(x) == 666)
    @assert(valueof(55) == 55)
    @assert(valueof'string("666") == "666")
}
