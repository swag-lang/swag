struct AA { x, y: s32 = 10 }

impl AA
{
    #[Swag.Inline]
    func titi(me)->{ a: f32, b: f32 }
    {
        var _a = cast(f32) me.x / 10
        var _b = cast(f32) me.y / 10
        return {_a, _b}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA
    var ew = x.titi()
    @assert(x.x == 10)
    @assert(x.y == 10)
    //@print(cast(f64) x.titi().a); @print("\n")
    //@print(cast(f64) x.titi().b); @print("\n")
}
