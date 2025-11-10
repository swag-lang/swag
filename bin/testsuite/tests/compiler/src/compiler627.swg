struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func opIndexAffect(me, i: u64, v: s32)
    {
        if (i == 0) do
            me.x = v
        else do
            me.y = v
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    pt[0] = 5
    pt[1] = 10
    @assert(pt.x == 5)
    @assert(pt.y == 10)
}
