struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func(op: string) opAssign(me, pt: Point)
    {
        if op == "<<="
        {
            me.x = pt.x
            me.y = pt.y
        }
        elif op == ">>="
        {
            me.x = pt.x
            me.y = pt.y
        }
        else
        {
            @assert(false)
        }
    }
}

#test
{
    var pt:  Point
    var pt1: Point
    pt1.x = 5
    pt1.y = 10
    pt <<= pt1
    pt1 >>= pt
}
