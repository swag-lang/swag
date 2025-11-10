//#[Swag.PrintBc]
#test
{
    let a8: u8 = 0
    @assert(~a8 == 0xFF)
    let a16: u16 = 0
    @assert(~a16 == 0xFFFF)
    let a32: u32 = 0
    @assert(~a32 == 0xFFFF_FFFF)
    let a64: u64 = 0
    @assert(~a64 == 0xFFFF_FFFF_FFFF_FFFF)

    let sa8: s8 = 0
    @assert(cast #bit (u8) ~sa8 == 0xFF)
    let sa16: s16 = 0
    @assert(cast #bit (u16) ~sa16 == 0xFFFF)
    let sa32: s32 = 0
    @assert(cast #bit (u32) ~sa32 == 0xFFFF_FFFF)
    let sa64: s64 = 0
    @assert(cast #bit (u64) ~sa64 == 0xFFFF_FFFF_FFFF_FFFF)
}
