struct AA
{
    index: s32
}

struct BB
{
    aa: AA
}

impl BB
{
    #[Swag.inline]
    method opIndex(idx: uint)->*AA
    {
        return &aa;
    }
}

//#[Swag.PrintBc]
#test
{
    var x: BB
    x[0].index = 10
}