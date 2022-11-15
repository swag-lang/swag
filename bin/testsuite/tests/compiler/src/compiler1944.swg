func foozle(x: any)
{
    isType := @kindof(x).flags & Swag.TypeInfoFlags.PointerTypeInfo
    @assert(isType ? true : false)

    ptr := cast(const *Swag.TypeInfo) @dataof(x)
    isInteger := ptr.flags & Swag.TypeInfoFlags.Integer
    @assert(isInteger ? true : false)
    isUnsigned := ptr.flags & Swag.TypeInfoFlags.Unsigned
    @assert(isUnsigned ? false : true)

    @assert(ptr.sizeof == 4)
    @assert(ptr.fullname == "s32")
}

#test
{
    foozle(s32)
}