struct A {
    buf: ^u8
    len: u32 = 4
}

impl A
{
    #[Swag.Inline]
    mtd toto()->string
    {
        return @mkstring(buf, len)
    }

    mtd titi(x, y: string)
    {
        @print(x)
    }

    //#[Swag.PrintBc]
    mtd tata()
    {
        titi(toto(), "")
    }
}

#test
{
    var x: A
    x.tata()
}