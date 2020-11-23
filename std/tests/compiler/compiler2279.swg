using swag

struct AA
{
    x: IAllocator
}

func toto(v: const &IAllocator)
{
}

//#[swag.printbc]
#test
{
    var v: AA
    toto(v.x)
}