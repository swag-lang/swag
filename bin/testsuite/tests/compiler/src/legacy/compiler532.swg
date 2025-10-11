struct point
{
    x:     s32
    y:     s32
}

impl point
{
    //#[Swag.PrintBc]
    func set(me, _x: s32, _y: s32)
    {
        me.x = _x
        me.y = _y
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: point
    point.set(pt, 10, 20)
    @assert(pt.x == 10)
    @assert(pt.y == 20)
}
