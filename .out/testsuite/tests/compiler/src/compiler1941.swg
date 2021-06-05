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
    func hash(using self)->u32
    {
        return val
    }
}

//#[Swag.printbc]
func(T) hash(value: T)->u32
{
    #if @typeof(T) == u32
        return value
    #elif @typeof(T).kind == Swag.TypeInfoKind.Struct
    {
        itf := cast(IHash) value
        return itf.hash()
    }
    //#else
    //    #assert(false, "cannot hash type " ~ @typeof(T).name)
    return 5
}

#test
{
    @assert(hash(555'u32) == 555)
    @assert(hash(444'f32) == 5)

    var x: AA
    @assert(hash(x) == 1971)
}