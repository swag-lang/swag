struct Toto { x, y: s32 = 10 }

impl Toto
{
    #[Swag.Implicit]
    func opCast(me)->s32
    {
        return me.x + me.y
    }
}

#test
{
    var x: Toto
    let y: s32 = x
    @assert(y == 20)
}
