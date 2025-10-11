struct AA
{
    x:   s32 = 555
}

impl AA
{
    mtd titi()
    {
        @assert(me.x == 555)
    }

    mtd toto()
    {
        @assert(me.x == 666)

        var v: AA
        with v
        {
            @assert(.x == 555)
            .titi()
        }
    }
}

#test
{
    var v: AA{666}
    v.toto()
}
