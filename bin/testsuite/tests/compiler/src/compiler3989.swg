struct AA
{
    val: u32 = 1971
}

interface IHash
{
    func hash(self)->u32
}

impl IHash for AA
{
    func impl hash(using self)->u32
    {
        return val
    }
}

//#[Swag.PrintBc]
func hash(value: const ref AA)
{
    var itf = cast(IHash) value
}
