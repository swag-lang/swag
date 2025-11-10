enum RGB { R, G, B }
enum RGB1 { G }

struct X {}

impl X
{
    mtd toto(k: RGB, k1: RGB1)
    {
        @assert(cast(s32) k == 1)
        @assert(cast(s32) k1 == 0)
    }

    mtd toto1()
    {
        me.toto(.G, .G)
    }
}

#test
{
    var x: X
    x.toto1()
}
