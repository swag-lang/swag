#[swag.align(16)]
struct VV
{
    v: [4] f32
}

#[swag.safety(false)]
func set(s: *f32, v: f32)
{
    loop i: 4 s[i] = v
}

#[swag.safety(false)]
func add(a, b, c: *f32)
{
    a[0] = b[0] + c[0]
    a[1] = b[1] + c[1]
    a[2] = b[2] + c[2]
    a[3] = b[3] + c[3]
}

//#[swag.printbc]
func toto(value: f32)
{
    var vv1: [4] f32
    var vv2: [4] f32
    var vv3: [4] f32

    set(&vv1[0], value)
    set(&vv2[0], value)
    add(&vv3[0], &vv1[0], &vv2[0])

    @print(cast(f64) vv3[0])
    @print(cast(f64) vv3[1])
    @print(cast(f64) vv3[2])
    @print(cast(f64) vv3[3])
}

#test
{
    toto(1)
}