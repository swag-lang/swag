
#global testerror

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

//#[Swag.printbc]
#test
{
    var v: BB
    @print(cast(s64) (&v.z - &v.v.x))
}
