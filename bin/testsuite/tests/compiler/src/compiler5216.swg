func toto(x: &any)
{
    let ptr = cast(*s32) @dataof(x)
    @assert(dref ptr == 42)
}

#test
{
    var x: any = 42
    toto(&x)
    @assert(x == s32)
}
