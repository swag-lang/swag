func foozle(x: any)
{
    @assert(@kindof(x).name == "const *swag.TypeInfoNative")
    ptr := cast(const *swag.TypeInfoNative) @dataof(x)
    @assert(ptr.name == "s32")
}

#test
{
    foozle(s32)
}