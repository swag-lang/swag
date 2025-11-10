struct Point
{
    x:     s32
    y:     s32
}

impl Point
{
    func opIndex(me, i, j: u64)->s32
    {
        if (i == 0) do
            return cast() j
        return cast() (j * 2)
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    pt.x = 10
    pt.y = 20
    @assert(pt[0, 100] == 100)
    @assert(pt[1, 100] == 200)
}
