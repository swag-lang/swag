//#[Swag.PrintBc]
#test
{
    let index: u32 = 65
    #[Swag.CanOverflow(true)]
    let val = index >> 3
    let bit = index - (val << 3)
    let shift: u8 = cast(u8) (1 << bit)
    @assert(shift == 2)
    @assert(cast(s16) shift == 2)
    @assert(cast(s32) shift == 2)
    @assert(cast(s64) shift == 2)

    @assert(cast(u16) shift == 2)
    @assert(cast(u32) shift == 2)
    @assert(cast(u64) shift == 2)
}