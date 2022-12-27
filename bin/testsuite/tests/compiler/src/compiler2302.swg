#global testerror

using Swag

struct AA
{
    x: IAllocator
}

impl AA
{
    func toto(self)
    {
        //x.alloc(null)
    }
}

func titi()
{
        var x: *u8
    dref x = 1

}
//#[Swag.PrintBc]
#test
{
    titi()
}
