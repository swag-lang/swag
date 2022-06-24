func toto(x: any)
{
    ptr := cast(*s32) @dataof(x)
    @assert(ptr[0] == 1)
    @assert(ptr[1] == 2)

    type := @kindof(x)
    @assert(type.kind == .Array)
    arr := cast(const *Swag.TypeInfoArray) type
    @assert(arr.count == 2)
}

#test
{
    toto(@[1, 2])
}