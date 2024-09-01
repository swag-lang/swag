struct AA
{
    val: u32 = 1971
}

interface IHash
{
    func hash(self)->u32;
}

impl IHash for AA
{
    func impl hash(using self)->u32
    {
        return val
    }
}

//#[Swag.PrintBc]
func(T) hash(value: T)->u32
{
    #if #typeof(T) == u32:
        return value
    #elif #typeof(T).kind == Swag.TypeInfoKind.Struct
    {
        let itf = cast(IHash) value
        return itf.hash()
    }
    return 5
}

#test
{
    @assert(hash(555'u32) == 555)
    @assert(hash(444'f32) == 5)

    var x: AA
    @assert(hash(x) == 1971)
}