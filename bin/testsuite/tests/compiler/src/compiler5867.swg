struct V2 { x: s32 = 1 }

impl V2
{
    mtd titi()
    {
        @assert(me.x == 10)
    }

    mtd toto()
    {
        var f: func||()
        f = mtd||()
        {
            me.titi()
        }

        f()
    }
}

#test
{
    var a: V2
    a.x = 10
    a.toto()
}
