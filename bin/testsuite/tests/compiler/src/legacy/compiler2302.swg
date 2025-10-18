#global testerror "Err0090"

using Swag

struct AA
{
    x:   IAllocator
}

impl AA
{
    func toto(me)
    {
        x.alloc(null)
    }
}

//#[Swag.PrintBc]
#test
{
}
