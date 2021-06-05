func foozle(x: any)
{
    @assert(@kindof(x).name == "const *Swag.TypeInfoNative")
    ptr := cast(const *Swag.TypeInfoNative) @dataof(x)
    @assert(ptr.name == "s32")
}

#test
{
    foozle(s32)
}