#global testerror "Err0023"

struct Toto {}

impl Toto
{
    //#[Swag.Implicit]
    func opCast(me)->s32
    {
        return 666
    }
}

#test
{
    var x: Toto
    var y: s32 = x
    @assert(y == 666)
}
