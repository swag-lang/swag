struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func opIndex(me, i: u64)->s32
    {
        if (i == 0) do
            return me.x
        return me.y
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    pt.x = 10
    pt.y = 20
    @assert(pt[0] == 10)
    @assert(pt[1] == 20)
}
