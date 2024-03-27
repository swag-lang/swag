using Swag

//#[Swag.PrintBc]
#test
{
    let x: any = 6
    let y: s32 = cast(s32) x
    @assert(cast(s32) x == 6)
    @assert(y == 6)
}
