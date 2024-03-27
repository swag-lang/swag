struct A
{
    x: s64 = 1
}

var G = 0

impl A
{
    func opDrop(self)
    {
        //@print("drop\n")
    }

    func opPostMove(self)
    {
        //@print("postmove\n")
        G = 1
    }

    func opPostCopy(self)
    {
        @assert(false)
    }
}

//#[Swag.PrintBc]
func toto() -> {a:A, b:bool}
{
    var x: A
    x.x = 555
    return {x, false}
}

//#[Swag.PrintBc, Swag.nooptim]
#test
{
    var v = toto()
    @assert(v.item0.x == 555)
    @assert(G == 1)
}