struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func(op: string) opAssign(me, pt: s32) {}
}

#test
{
    var pt: Point
    pt &= 1
    pt |= 1
    pt ^= 1
    pt %= 1
    pt *= 1
}
