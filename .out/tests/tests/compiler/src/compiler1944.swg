func foozle(x: any)
{
    isType := @kindof(x).flags & swag.TypeInfoFlags.TypeInfoPtr
    @assert(isType ? true : false)

    ptr := cast(const *swag.TypeInfo) @dataof(x)
    isInteger := ptr.flags & swag.TypeInfoFlags.Integer
    @assert(isInteger ? true : false)
    isUnsigned := ptr.flags & swag.TypeInfoFlags.Unsigned
    @assert(isUnsigned ? false : true)

    @assert(ptr.sizeof == 4)
    @assert(ptr.name == "s32")
}

#test
{
    foozle(s32)
}