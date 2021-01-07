#[swag.align(16)]
struct VV
{
    v: [4] f32
}

func add(s: *VV, v: f32)
{
    s.v[0] += v
    s.v[1] += v
    s.v[2] += v
    s.v[3] += v
}

//#[swag.printbc]
#test
{
    var b: bool
    //#[swag.align(16)]
    var vv: VV
    add(&vv, 1)
    @print(cast(f64) vv.v[0])
    @print(cast(f64) vv.v[1])
    @print(cast(f64) vv.v[2])
    @print(cast(f64) vv.v[3])
}