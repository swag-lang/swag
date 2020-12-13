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

//#[swag.printbc]
#test
{
    var v: BB
    @print(cast(s64) (cast(*u8) &v.z - cast(*u8) &v.v.x))
}
