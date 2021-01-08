#[swag.align(16)]
struct VV
{
    v: [4] f32
}

//#[swag.printbc]
#[swag.safety(false)]
func add(xx: string, s: *f32, v: f32)
{
    s[0] += v
    s[1] += v
    s[2] += v
    s[3] += v
}

var G = 100
//#[swag.printbc]
#test
{
    var vv: [4] f32
    loop G
    add("toto", &vv[0], 1)

    @print(cast(f64) vv[0])
    @print(cast(f64) vv[1])
    @print(cast(f64) vv[2])
    @print(cast(f64) vv[3])
}