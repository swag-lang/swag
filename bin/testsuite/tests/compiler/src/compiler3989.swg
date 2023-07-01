struct AA
{
    val: u32 = 1971
}

interface IHash
{
    hash: func(self)->u32
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
    itf := cast(IHash) value
}
