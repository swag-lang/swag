struct AA
{
    index: s32
}

struct BB
{
    aa:  AA
}

impl BB
{
    #[Swag.Inline]
    mtd opIndex(idx: u64)->*AA
    {
        return &me.aa
    }
}

//#[Swag.PrintBc]
#test
{
    var x: BB
    x[0].index = 10
}
