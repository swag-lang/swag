func foozle(x: any)
{
    @assert(@kindof(x).fullname == "const *Swag.TypeInfoNative")
    var ptr = cast(const *Swag.TypeInfoNative) @dataof(x)
    @assert(ptr.fullname == "s32")
}

#test
{
    foozle(s32)
}