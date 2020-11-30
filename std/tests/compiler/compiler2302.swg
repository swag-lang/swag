#module "compiler2302"
#unittest error

using swag

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

//#[swag.printbc]
#test
{
}
