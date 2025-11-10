struct A
{
    x:   s64 = 1
}

var G = 0

impl A
{
    func opDrop(me)
    {
        //@print("drop\n")
    }

    func opPostMove(me)
    {
        //@print("postmove\n")
        G = 1
    }

    func opPostCopy(me)
    {
        @assert(false)
    }
}

//#[Swag.PrintBc]
func toto()->A
{
    var x: A
    x.x = 555
    return x
}

//#[Swag.PrintBc]
#test
{
    var v = toto()
}
