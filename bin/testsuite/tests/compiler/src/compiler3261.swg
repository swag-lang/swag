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
            name = "1"
        }
    }
}
