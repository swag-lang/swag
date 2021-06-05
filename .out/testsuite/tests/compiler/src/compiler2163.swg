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

//#[Swag.printbc]
func toto() -> A
{
    var x: A
    x.x = 555
    return x
}

//#[Swag.printbc]
#test
{
    v := toto()
}