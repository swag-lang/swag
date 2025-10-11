struct A
{
    x:   const [*] u8
}

impl A
{
    #[Swag.Inline]
    mtd read()
    {
        me.x += 1
    }

    mtd test()
    {
        me.read()
    }
}
