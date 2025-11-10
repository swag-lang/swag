struct A { x: s32 }

impl A
{
    mtd toto()
    {
        me.x = 0
    }
}

#test
{
    var a: A
    a.toto()
}
