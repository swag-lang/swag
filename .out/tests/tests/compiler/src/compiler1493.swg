//#[Swag.printbc]
#test
{
    var a8: u8 = 0
    @assert(~a8 == 0xFF)
    var a16: u16 = 0
    @assert(~a16 == 0xFFFF)
    var a32: u32 = 0
    @assert(~a32 == 0xFFFF_FFFF)
    var a64: u64 = 0
    @assert(~a64 == 0xFFFF_FFFF_FFFF_FFFF)

    var sa8: s8 = 0
    @assert(bitcast(u8) ~sa8 == 0xFF)
    var sa16: s16 = 0
    @assert(bitcast(u16) ~sa16 == 0xFFFF)
    var sa32: s32 = 0
    @assert(bitcast(u32) ~sa32 == 0xFFFF_FFFF)
    var sa64: s64 = 0
    @assert(bitcast(u64) ~sa64 == 0xFFFF_FFFF_FFFF_FFFF)
}