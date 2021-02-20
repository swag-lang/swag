struct(T) AA
{
    ptr: *T
}

impl AA
{
    func opCast(using self)->const [..] T
    {
        return @mkslice(ptr, 0)
    }

    func opCast(using self)->[..] T
    {
        return @mkslice(ptr, 0)
    }
}

#test
{
    var x: AA's32
    ptr := cast(const [..] s32) x
    ptr1 := cast([..] s32) x
}