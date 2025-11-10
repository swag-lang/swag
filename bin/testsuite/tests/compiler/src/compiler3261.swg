struct BB
{
    name: string
}

struct AA
{
    using bb: BB
}

impl AA
{
    mtd toto()
    {
        var v: AA
        with v
        {
            me.name = "1"
        }
    }
}
