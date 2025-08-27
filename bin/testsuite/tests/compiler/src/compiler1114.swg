#test
{
    let y: u32 = 0xFFFFFFFF
    let x: s32 = cast #bit (s32) y
    let z: s64 = cast(s64) x
    @assert(z == -1)
}
