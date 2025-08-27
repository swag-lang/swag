struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func(op: string) opAssign(me, what: s32)
    {
        if op == "*="
        {
            me.x = what
            me.y = what
        }

        if op == "+="
        {
            me.x += what
            me.y += what
        }

        if op == "-="
        {
            me.x -= what
            me.y -= what
        }
    }
}

#test
{
    var pt: Point
    pt *= 10
    @assert(pt.x == 10)
    @assert(pt.y == 10)
    pt += 5
    @assert(pt.x == 15)
    @assert(pt.y == 15)
    pt -= 4
    @assert(pt.x == 11)
    @assert(pt.y == 11)
}
