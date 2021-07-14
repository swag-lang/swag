struct A
{
    x: s32
    y: s32
}

//#[Swag.PrintBc]
func toto1()->A
{
    return @{y: 555, x: 444}
}

#test
{
    res := toto1()
    @assert(res.x == 444)
    @assert(res.y == 555)
}