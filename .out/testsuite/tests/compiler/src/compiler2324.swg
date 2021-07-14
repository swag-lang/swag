struct AA
{
    x: u64
    y: u16
}

struct BB
{
    v: AA
    z: u32 = 555
}

//#[Swag.PrintBc]
#test
{
    var v: BB
    @assert(cast(s64) (cast(*u8) &v.z - cast(*u8) &v.v.x) == 16)
    @assert(@sizeof(BB) == 24)
}
