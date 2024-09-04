#global testerror "Err0674"

using Swag

struct AA
{
    x: IAllocator
}

impl AA
{
    func toto(self)
    {
        x.alloc(null)
    }
}

//#[Swag.PrintBc]
#test
{
}
