struct point
{
    x:     s32 = 60
    y:     s32
}

impl point
{
    func toto(me)
    {
        @assert(me.x == 60)
    }
}

//#[Swag.PrintBc]
#test
{
    var pt: point
    pt.toto()
}
