struct Toto { x, y: s32 = 10 }

impl Toto
{
    #[Swag.Implicit, Swag.Overload]
    func opCast(me)->s32
    {
        return me.x + me.y
    }

    #[Swag.Implicit, Swag.Overload]
    func opCast(me)->string
    {
        return "x + y"
    }
}

//#[Swag.PrintBc]
#test
{
    var x: Toto
    let y: s32 = x
    @assert(y == 20)

    let z: string = x
    @assert(z == "x + y")
}
