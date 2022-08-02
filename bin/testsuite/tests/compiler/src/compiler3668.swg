struct A
{
    x: const ^u8
}

impl A
{
    #[Swag.Inline]
    mtd read()
    {
        x += 1
    }

    mtd test()
    {
        read()
    }
}