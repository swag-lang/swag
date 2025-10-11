func toto(x: any)
{
    let ptr = cast([*] s32) @dataof(x)
    @assert(ptr[0] == 1)
    @assert(ptr[1] == 2)
}

#test
{
    toto({1, 2})
}
