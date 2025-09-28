using Swag

//#[Swag.PrintBc]
#test
{
    let x: any = 6
    let y: s32 = x
    @assert(cast(s32) x == 6)
    @assert(cast(s32) y == 6)
}
