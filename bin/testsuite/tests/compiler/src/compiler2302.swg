#global testerror "Err0718"

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
