struct Point
{
    c:   u32 = 5
}

impl Point
{
    func opCount(me)->u64
    {
        return cast(u64) me.c
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: Point
    var cpt = 0
    for pt
    {
        cpt += 1
    }
    @assert(cpt == 5)
}
