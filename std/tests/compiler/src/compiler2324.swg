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

struct CaseRange
{
    lo, hi: char
    delta:  [3] s32
}

//#[swag.printbc]
#test
{
    @print(cast(s64) @sizeof(CaseRange))
    var v: BB
    @print(cast(s64) (cast(*u8) &v.z - cast(*u8) &v.v.x))
}
