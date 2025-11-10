struct toto
{
    x:     s32
    y:     s32
}

impl toto
{
    func fff(me)
    {
        @assert(me.x == 100)
        @assert(me.y == 200)
    }
}

//#[Swag.PrintBc]
#test
{
    using toto
    var pt: toto
    pt.x = 100
    pt.y = 200
    fff(pt)
}
