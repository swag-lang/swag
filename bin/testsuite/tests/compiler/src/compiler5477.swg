struct Toto { x, y: s32 = 10 }

impl Toto
{
    #[Swag.Implicit, Swag.Inline]
    func opCast(me)->string
    {
        return "x + y"
    }
}

//#[Swag.PrintBc]
#test
{
    var x: Toto
    let z: string = x
    @assert(z == "x + y")
}
