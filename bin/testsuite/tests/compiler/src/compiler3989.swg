struct AA
{
    val: u32 = 1971
}

interface IHash
{
    func hash(me)->u32;
}

impl IHash for AA
{
    func impl hash(me)->u32
    {
        return me.val
    }
}

//#[Swag.PrintBc]
func hash(value: const &AA)
{
    var itf = cast(IHash) value
}
