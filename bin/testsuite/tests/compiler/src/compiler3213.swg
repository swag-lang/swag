struct Base
{
    itf: IBase
}

interface IBase
{
    func check(self)->s32;
}

impl IBase for Base
{
    mtd impl check()->s32
    {
        return 1
    }
}

impl Base
{
    //#[Swag.PrintBc, Swag.Safety("", false)]
    mtd toto()->s32
    {
        return itf.check()
    }
}

struct Child
{
    using base: Base
}

impl IBase for Child
{
    mtd impl check()->s32
    {
        return 2
    }
}

#test
{
    var base: Base
    base.itf = cast(IBase) base
    @assert(base.toto() == 1)

    var child: Child
    child.itf = cast(IBase) child
    @assert(child.toto() == 2)
}
