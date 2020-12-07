struct A
{
    x: s32
    y: s32
}

//#[swag.printbc, swag.optim(false)]
func toto1()->A
{
    var z: retval{y: 555, x: 444}
    return z
}

#test
{
    res := toto1()
    @assert(res.x == 444)
    @assert(res.y == 555)
}