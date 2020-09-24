struct AA
{
    x, y: s32 = 10
}

impl AA
{
    #[swag.inline]
    func titi(using self)->{a: f32, b: f32}
    {
        _a := cast(f32) x / 10
        _b := cast(f32) y / 10
        return @{_a, _b}
    }
}

//#[swag.printbc]
#test
{
    var x: AA
    x.titi()
    @assert(x.x == 10)
    @assert(x.y == 10)
    //@print(cast(f64) x.titi().a); @print("\n")
    //@print(cast(f64) x.titi().b); @print("\n")
}