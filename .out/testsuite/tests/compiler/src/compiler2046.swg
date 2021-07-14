struct AA
{
    x: s32 = 10
    y: s32 = 20
}

impl AA
{
    #[Swag.Inline]
    func titi(using self)->{a: s32, b: s32}
    {
        _a := x / 10
        _b := y / 10
        return @{_a, _b}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA
    @assert(x.titi().a == 1)
    @assert(x.titi().b == 2)
    @assert(x.x == 10)
    @assert(x.y == 20)
}