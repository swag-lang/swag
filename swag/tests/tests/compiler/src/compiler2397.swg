#[swag.align(16)]
struct VV
{
    v: [4] f32
}

//#[swag.printbc]
#test
{
    var b: bool
    //#[swag.align(16)]
    var vv: VV
    vv.v[0] = 1
    vv.v[1] = 1
    vv.v[2] = 1
    vv.v[3] = 1
    @print(cast(f64) vv.v[0])
    @print(cast(f64) vv.v[1])
    @print(cast(f64) vv.v[2])
    @print(cast(f64) vv.v[3])
}