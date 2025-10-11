struct(T) AA
{
    ptr: [*] T
}

impl AA
{
    #[Swag.Overload]
    func opCast(me)->const [..] T
    {
        return @mkslice(me.ptr, 0)
    }

    #[Swag.Overload]
    func opCast(me)->[..] T
    {
        return @mkslice(me.ptr, 0)
    }
}

#test
{
    var x:   AA's32
    var ptr  = cast(const [..] s32) x
    var ptr1 = cast([..] s32) x
}
