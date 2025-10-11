struct Point
{
    x:     s32
    y:     s32
}

//#[Swag.PrintBc]
#test
{
    var pt1: Point
    var pt2: Point
    pt2.x = 1
    @assert(pt1 < pt2)
    @assert(pt2 > pt1)
    pt1.x = 10
    @assert(pt1 > pt2)
    @assert(pt2 < pt1)

    @assert(pt2 <= pt2)
}

impl Point
{
    func opCmp(me, other: Point)->s32
    {
        if me.x < other.x or me.y < other.y do
            return -1
        if me.x > other.x or me.y > other.y do
            return 1
        return 0
    }
}
