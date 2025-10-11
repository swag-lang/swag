var G = 0

interface Toto
{
    func print(me)
    {
        G = 1
    }
}

struct A {}

impl Toto for A
{
}

//#[Swag.PrintAsm]
#test
{
    var a: A
    let i  = cast(Toto) a
    i.print()
    @assert(G == 1)
}
