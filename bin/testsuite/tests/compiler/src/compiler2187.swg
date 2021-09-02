//#[Swag.PrintBc]
func toto()->{x: s32, y: s32}
{
    var res: retval
    res.x = 20
    return res
}

//#[Swag.PrintBc]
#test
{
    v := toto()
    @assert(v.x == 20)
    @assert(v.y == 0)
}