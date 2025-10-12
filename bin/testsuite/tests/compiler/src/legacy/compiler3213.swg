struct Base
{
    itf: IBase
}

interface IBase
{
    func toCheck(me)->s32;
}

impl IBase for Base
{
    mtd impl toCheck()->s32
    {
        return 1
    }
}

impl Base
{
    //#[Swag.PrintBc, Swag.Safety(.All, .All, false)]
    mtd toto()->s32
    {
        return me.itf.toCheck()
    }
}

struct Child
{
    using base: Base
}

impl IBase for Child
{
    mtd impl toCheck()->s32
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
