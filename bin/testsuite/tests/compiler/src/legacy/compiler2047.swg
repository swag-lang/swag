struct AA
{
    x:     s32 = 10
    y:     s32 = 20
}

impl AA
{
    #[Swag.Inline]
    func titi(me)->{ a: s32, b: s32 }
    {
        var _a = me.x / 10
        var _b = me.y / 10
        return {_a, _b}
    }
}

//#[Swag.PrintBc]
#test
{
    var x: AA
    @assert(x.titi().a == 1)
    @assert(x.titi().b == 2)
    @assert(x.titi().a == 1)
    @assert(x.titi().b == 2)
    @assert(x.titi().a == 1)
    @assert(x.titi().b == 2)
    @assert(x.titi().a == 1)
    @assert(x.titi().b == 2)
    @assert(x.titi().a == 1)
    @assert(x.titi().b == 2)
}
