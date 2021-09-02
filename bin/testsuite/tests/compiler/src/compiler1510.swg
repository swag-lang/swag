//#[Swag.PrintBc]
#test
{
    /*var index: u32 = 65
    val := index >> 3
    bit := index - (val << 3)*/
    var shift: u8 = 2 //cast(u8) (1 << bit)
    @assert(shift == 2)
    @assert(cast(s16) shift == 2)
    @assert(cast(s32) shift == 2)
    @assert(cast(s64) shift == 2)

    @assert(cast(u16) shift == 2)
    @assert(cast(u32) shift == 2)
    @assert(cast(u64) shift == 2)
}