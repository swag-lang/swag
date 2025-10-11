struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func(op: string) opAssign(me, what: s32)
    {
        if op == "+="
        {
            me.x = what
            me.y = what * 2
        }
    }
}

#test
{
    var pt: Point
    pt += 10
    @assert(pt.x == 10)
    @assert(pt.y == 20)
}
