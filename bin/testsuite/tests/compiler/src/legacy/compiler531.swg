struct point
{
    x:   s32
}

impl point
{
    //#[Swag.PrintBc]
    func set(me, _x: s32)
    {
        @assert(me.x == 100)
        me.x = _x
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: point
    pt.x = 100
    point.set(pt, 10)
    @assert(pt.x == 10)
}
