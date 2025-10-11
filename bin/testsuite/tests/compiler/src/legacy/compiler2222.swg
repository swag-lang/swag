struct A
{
    x:     s32
    y:     s32
}

//#[Swag.PrintBc, Swag.Optimize(false)]
#[Swag.Inline]
func toto1()->A
{
    var z: retval{y: 555, x: 444}
    return z
}

#test
{
    let res = toto1()
    @assert(res.x == 444)
    @assert(res.y == 555)
}
