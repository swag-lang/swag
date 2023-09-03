#global testerror "Err0112"

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
