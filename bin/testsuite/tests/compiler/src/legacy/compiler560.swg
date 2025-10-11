struct point
{
    x:     s32 = 60
    y:     s32 = 70
}

impl point
{
    func toto(me)
    {
        @assert(me.x == 60)
        @assert(me.y == 70)
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: [2] point
    pt[0].toto()
}
