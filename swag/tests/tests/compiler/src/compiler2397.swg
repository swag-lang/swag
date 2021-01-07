//#[swag.printbc]
#test
{
    var b: bool
    #[swag.align(16)]
    var v: [4] f32
    v[0] = 1
    v[1] = 1
    v[2] = 1
    v[3] = 1
    @print(cast(f64) v[0])
    @print(cast(f64) v[1])
    @print(cast(f64) v[2])
    @print(cast(f64) v[3])
}