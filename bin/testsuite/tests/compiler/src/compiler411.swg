
//#[Swag.PrintBc]
#test
{
    let c  = [666]
    let d: [..] s32 = cast([..] s32) c
    @assert(d[0] == 666)
    @assert(@countof(d) == 1)
}
