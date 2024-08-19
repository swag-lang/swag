#global testerror "Err0559"

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
    @print(cast(s64) (cast(^u32) &v.z - cast(^u64) &v.v.x))
}
