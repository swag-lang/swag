func foozle(x: any)
{
    let isType = @kindof(x).flags & Swag.TypeInfoFlags.PointerTypeInfo
    @assert(isType ? true : false)

    let ptr       = cast(const *Swag.TypeInfo) @dataof(x)
    let isInteger = ptr.flags & Swag.TypeInfoFlags.Integer
    @assert(isInteger ? true : false)
    let isUnsigned = ptr.flags & Swag.TypeInfoFlags.Unsigned
    @assert(isUnsigned ? false : true)

    @assert(ptr.sizeof == 4)
    @assert(ptr.fullname == "s32")
}

#test
{
    foozle(s32)
}
