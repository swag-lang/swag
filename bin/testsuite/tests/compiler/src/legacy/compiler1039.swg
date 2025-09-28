struct Point
{
    x:   s32
}

impl Point
{
    func opDrop(me)
    {
        me.x = 100
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    let ptr = &pt
    @drop(ptr)
    @assert(pt.x == 100)
}
