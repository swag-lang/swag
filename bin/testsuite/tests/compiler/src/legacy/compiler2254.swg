struct A
{
    x    = 1
}

struct B
{
    x:   [1] A
}

impl B
{
    func opDrop(me)
    {
        //@print("drop")
    }
}

//#[Swag.PrintBc]
#test
{
    var res: [1] B
    @assert(res[0].x[0].x == 1)
}
