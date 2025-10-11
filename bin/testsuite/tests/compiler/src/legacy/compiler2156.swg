#global testerror "Err0480"

struct A {}

impl A
{
    #[Swag.Overload]
    func toto(me, x: A)->s32
    {
        return 1
    }

    #[Swag.Overload]
    func toto(x, y: A)->s32
    {
        return 2
    }
}

#test
{
    var a, b: A
    @assert(a.toto(a, b) == 1)
}
