struct A
{
    buf:     [*] u8
    len:     u32 = 4
}

impl A
{
    #[Swag.Inline]
    mtd toto()->string
    {
        return @mkstring(me.buf, me.len)
    }

    mtd titi(x, y: string)
    {
        //@print(x)
    }

    //#[Swag.PrintBc]
    mtd tata()
    {
        me.titi(me.toto(), "")
    }
}

#test
{
    var x: A
    x.tata()
}
